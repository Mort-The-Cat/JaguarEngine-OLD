#ifndef JAGUAR_COMPRESSION
#define JAGUAR_COMPRESSION

#include<vector>

#include "../Controllers/Job_System.h"

// IMPORTANT: This compression only works on L-Endian systems !!!!

namespace Openzip
{

	struct Compressor_Data
	{
		std::vector<uint8_t> Instructions;
		std::vector<uint32_t> Raw_Data;
	};

	// Perhaps I should encode some chunk information into the file..........

	// That will allow some basic multithreading for decompression


	template<size_t Bytes>
	inline uint32_t Get_Value(const std::vector<uint8_t>& Raw_Data, size_t Index)
	{
		uint32_t Word = 0;
		size_t Byte = 0;

		while (Byte < Bytes)
			Word |= Raw_Data[Index++] << (8 * Byte++);

		//while (Bytes--)
			//Word >>= 8;
			//Word |= Raw_Data[Index++] << (8 * Byte++);
		//Word <<= 8;
		//Word |= Raw_Data[Index++];

		return Word;
	}

	void Push_Bytes(std::vector<uint8_t>& Bytes, size_t Value, size_t Count)
	{
		while (Count--)
		{
			Bytes.push_back(Value);

			Value >>= 8;				// Shifts away the lower byte
		}
	}

	// LSB set?
	//	copy instruction
	//	15-bit value (how many 32-bit words *backwards* we source the copy from
	//	16-bit value length

	//	LSB cleared?
	//	literal instruction
	//	15-bit value length
	//	*literals*

	void Decompress_Raw_Data(Compressor_Data& Compressor)
	{
		size_t Index = 8;

		Compressor.Raw_Data.resize(
			Get_Value<8>(Compressor.Instructions, 0)
		);

		size_t Write = 0;

		while (Index < Compressor.Instructions.size())
		{
			if (Compressor.Instructions[Index] & 0x01u) // check LSB...
			{
				// this is a copy instruction!

				int Offset = Get_Value<2>(Compressor.Instructions, Index) >> 1;
				Index += 2;
				size_t Length = Get_Value<2>(Compressor.Instructions, Index);
				Index += 2;

				size_t Start = Write - Offset;

				//while (Length--)
				//	Compressor.Raw_Data[Write++] = Compressor.Raw_Data[Start++];


				//Write += Length;

				if (Offset <= Length)
					while (Length--)
						Compressor.Raw_Data[Write++] = Compressor.Raw_Data[Start++];		// In certain cases, we can't use memcpy due to race conditions
				else
				{
					memcpy(Compressor.Raw_Data.data() + Write, Compressor.Raw_Data.data() + Start, Length * sizeof(uint32_t));
					Write += Length;
				}

				//
			}
			else
			{
				// this is a literal instruction!

				size_t Length = Get_Value<2>(Compressor.Instructions, Index) >> 1;
				Index += 2;

				memcpy(Compressor.Raw_Data.data() + Write, Compressor.Instructions.data() + Index, Length * sizeof(uint32_t));

				Index += Length * sizeof(uint32_t);

				Write += Length;
			}
		}
	}

	bool Compare(const std::vector<uint32_t>& Raw_Data, size_t Index, size_t Source, size_t Next_Chunk)
	{
		if (Index + 1 >= Next_Chunk)
			return false;

		return Raw_Data[Index] == Raw_Data[Source];
	}

	void Compress_Chunk(Compressor_Data& Compressor, std::vector<uint8_t>& Instructions_Out, size_t Chunk, size_t Next_Chunk)
	{
		// This will search for the longest run length in the buffer

		size_t Index = Chunk;

		size_t Int_Literals = 0;

		while (Index < Next_Chunk)
		{
			// this will search for the longest run-length

			size_t Longest_Length = 0;
			size_t Location = 0;// Index + Int_Literals > 32766 ? Int_Literals + Index - 32767 : 0; // Where to start?

			bool Location_Found = false;

			for (size_t Source =
				Index + Int_Literals > 32766 ? Int_Literals + Index - 32767 : 0;
				Source < Index + Int_Literals; Source++)
			{
				size_t Length = 0;

				while (Compare(Compressor.Raw_Data, Index + Int_Literals + Length, Source + Length, Next_Chunk))
					Length++;

				if (Length >= Longest_Length)
				{
					Location_Found = true;

					Longest_Length = Length;
					Location = Source;
				}
			}

			if (Longest_Length > 1 ||
				(Index + Int_Literals + Longest_Length >= Next_Chunk)
				)
			{
				if (Int_Literals)
				{
					// we want to write some literals........

					Push_Bytes(Instructions_Out, Int_Literals << 1, 2); // we're only copying 2 bytes because this is a 16-bit value

					if (Int_Literals > 32767)
						printf(" >> Potential overflow error of literal counts! %u\n", Int_Literals);


					// Push_Bytes(Compressor.Instructions, Location, 4);	// we're copying 4 bytes because this is a 32-bit value

					while (Int_Literals--)
						Push_Bytes(Instructions_Out, Compressor.Raw_Data[Index++], 4); // we're copying 4 bytes because this is a 32-bit value

					Int_Literals = 0;
				}
				if (Longest_Length > 32767u * 2u)
					printf(" >> Potential overflow error of length! %u\n", Longest_Length);

				// Then!! we want to copy the memory we're copying

				if (Location_Found)
				{
					//Push_Bytes(Instructions_Out, 0x01u | (Length << 1), 4); // we're only copying 4 bytes because this is a 32-bit value

					//Push_Bytes(Instructions_Out, Location, 4);	// 4-bytes, is a 32-bit word

					Push_Bytes(Instructions_Out, 0x01u | ((Index - Location) << 1), 2); // 16-bit value

					Push_Bytes(Instructions_Out, Longest_Length, 2);		// 2-bytes, is a 16-bit word

					// That's all!

					Index += Longest_Length;
				}
				else
				{
					Int_Literals++;
				}
			}
			else
				Int_Literals++;
		}
	}

	struct Compress_Worker_Data
	{
		Compressor_Data& Compressor;
		std::vector<uint8_t>& Instructions_Out;
		size_t Chunk, Next_Chunk;
	};

	void Compress_Chunk_Worker(void* Datap)
	{
		Compress_Worker_Data* Data = (Compress_Worker_Data*)Datap;

		Compress_Chunk(Data->Compressor, Data->Instructions_Out, Data->Chunk, Data->Next_Chunk);

		printf("Finished chunk\n");

		delete Data;
	}

	void Compress_Raw_Data(Jaguar::Job_System* System, Compressor_Data& Compressor, size_t Chunk_Size = 0x8000)
	{
		std::vector<std::vector<uint8_t>> Instructions_Out(std::ceil((float)Compressor.Raw_Data.size() / Chunk_Size) - 1);

		for (size_t Chunk = Chunk_Size, Job = 0; Chunk < Compressor.Raw_Data.size(); Chunk += Chunk_Size, Job++)
		{
			Jaguar::Submit_Job(System, Jaguar::Job(
				new Compress_Worker_Data(
					{
						Compressor, Instructions_Out[Job], Chunk, std::min<size_t>(Chunk + Chunk_Size, Compressor.Raw_Data.size())
					}
				),

				Compress_Chunk_Worker
			));

			/*Compress_Chunk(Compressor, Compressor.Instructions, Chunk,
				std::min<size_t>(Chunk + Chunk_Size, Compressor.Raw_Data.size())
			);*/
		}

		// size_t Chunk_Counters = Instructions_Out.size() * sizeof(uint32_t); // number of chunks x sizeof(uint32_t)

		Push_Bytes(Compressor.Instructions, Compressor.Raw_Data.size(), 8);

		Compress_Chunk(Compressor, Compressor.Instructions, 0u,
			std::min<size_t>(Chunk_Size, Compressor.Raw_Data.size()));

		Jaguar::Wait_For_Job_System_Completion(System);


		for (size_t Job = 0; Job < Instructions_Out.size(); Job++)
			for (size_t Index = 0; Index < Instructions_Out[Job].size(); Index++)
				Compressor.Instructions.push_back(Instructions_Out[Job][Index]);
	}
}

#endif