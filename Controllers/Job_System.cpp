#include "Job_System.h"

namespace Jaguar
{
	void Worker_Set_Working(Job_System* System, size_t Worker, bool Value)
	{
		System->Workers[Worker].Job_Stack_Lock.lock();
		System->Workers[Worker].Working = Value;
		System->Workers[Worker].Job_Stack_Lock.unlock();
	}

	bool Worker_Get_Working(Job_System* System, size_t Worker)
	{
		bool Value;
		System->Workers[Worker].Job_Stack_Lock.lock();
		Value = System->Workers[Worker].Working;
		System->Workers[Worker].Job_Stack_Lock.unlock();

		return Value;
	}

	void Job_System_Set_Working(Job_System* System, bool Value)
	{
		for (size_t Index = 0; Index < System->Workers.size(); Index++)
			Worker_Set_Working(System, Index, Value);
	}

	void Worker_Begin(Job_System* System, const size_t Worker_Index)	// This can be a private function
	{
		System->Workers[Worker_Index].Finished = false;

		while (!Worker_Get_Working(System, Worker_Index));	// wait until we've been told to start working

		while (Worker_Get_Working(System, Worker_Index))
		{
			uint8_t Index = Worker_Index;

			Job Selected_Job;

			System->Workers[Index].Job_Stack_Lock.lock();

			while (!System->Workers[Index].Job_Stack.size())
			{
				System->Workers[Index].Job_Stack_Lock.unlock();

				if (!Worker_Get_Working(System, Worker_Index))
				{
					printf("Thread %zu has finished by sudden request\n", Worker_Index);
					return;
				}

				Index++;
				Index %= System->Workers.size();

				if (Index == Worker_Index)
				{
					System->Workers[Worker_Index].Job_Stack_Lock.lock();
					System->Workers[Worker_Index].Finished = true;
					System->Workers[Worker_Index].Job_Stack_Lock.unlock();
				}

				System->Workers[Index].Job_Stack_Lock.lock(); // But lock the next one while we check it!
			}

			System->Workers[Worker_Index].Finished = false;

			Selected_Job = System->Workers[Index].Job_Stack.back();		// Load up a job!
			System->Workers[Index].Job_Stack.pop_back();				// This is our job now- we can remove it from the list
			System->Workers[Index].Job_Stack_Lock.unlock();					// We don't need this job stack anymore right now- unlock it!

			Selected_Job.Job_Function(Selected_Job.Parameters); // This executes the selected task

			// Work endlessly...
		}

		printf("Thread %zu has finished\n", Worker_Index);
	}

	void Wait_For_Job_System_Completion(Job_System* Target_System)
	{
		bool Not_Completed;

		do
		{
			Not_Completed = false;

			for (size_t W = 0; W < Target_System->Workers.size(); W++)
			{
				Target_System->Workers[W].Job_Stack_Lock.lock();
				Not_Completed |= !Target_System->Workers[W].Finished;
				Target_System->Workers[W].Job_Stack_Lock.unlock();
			}
		} while (Not_Completed);

		//printf("It's decided we've finished\n");
	}

	void Terminate_Job_System(Job_System* Target_System)
	{
		Wait_For_Job_System_Completion(Target_System);		// wait until all current/queued jobs are finished

		Job_System_Set_Working(Target_System, false);		// tell every worker to terminate

		for (size_t W = 0; W < Target_System->Workers.size(); W++)
			Target_System->Worker_Threads[W].join();		// rejoin all of the worker threads

		Target_System->Worker_Threads.clear();				// destroys old worker threads (required)
		Target_System->Workers.clear();						// just resize this vector if necessary
	}

	void Initialise_Job_System(Job_System* Target_System, size_t Workers)	// only required if you want to create a new job system 
	{
		Terminate_Job_System(Target_System);

		Target_System->Last_Submitted_Job_Index = 0;

		Target_System->Workers = std::vector<Worker>(Workers);

		for (size_t Index = 0; Index < Workers; Index++)
		{
			Target_System->Worker_Threads.push_back(std::thread(Worker_Begin, Target_System, Index));
		}

		Job_System_Set_Working(Target_System, true);
	}

	void Submit_Job(Job_System* System, Job Task)
	{
		System->Job_Submission_Mutex.lock();

		size_t Index = System->Last_Submitted_Job_Index;

		System->Last_Submitted_Job_Index++;
		System->Last_Submitted_Job_Index %= System->Workers.size();

		System->Job_Submission_Mutex.unlock();

		System->Workers[Index].Job_Stack_Lock.lock();

		System->Workers[Index].Job_Stack.push_back(Task);

		System->Workers[Index].Job_Stack_Lock.unlock();

		//
	}
}