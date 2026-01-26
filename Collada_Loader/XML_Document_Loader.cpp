#include "Collada_Loader.h"

void Throw_Error(const char* Formatted_String)
{
	fprintf(stderr, Formatted_String, strerror(errno));
}

std::string Load_File_Contents(const char* Filename)
{
	std::ifstream File(Filename); // Start at end of file

	if (!File.is_open())
	{
		Throw_Error(" >> Fatal error! Unable to load file contents: %s\n");

		return ""; // nothing to return...
	}

	// Otherwise, we can continue

	File.seekg(0, std::ios::end);

	std::string Contents;
	size_t Size = File.tellg();

	File.seekg(0, std::ios::beg);

	Contents.resize(Size); // Fits buffer

	File.read((char*)Contents.data(), Size);

	File.close();

	return Contents;
}

namespace Collada
{
	long Search_String_Contents(const char* Characters, const char* Key) // Returns the index of the substring we're looking for, returns -1
	{
		const char* Pointer = strstr(Characters, Key);

		if (Pointer)
			return Pointer - Characters;

		return -1;
	}

	long Next_Nonwhitespace(const char* Characters)
	{
		long Index = 0;
		while (isspace(Characters[Index]))
			Index++;

		return Index;
	}

	void Get_XML_Properties(const std::string& Contents, long& Index, std::string& Property, const char* Property_Identifier)
	{
		long Delta_To_ID = Search_String_Contents(Contents.data() + Index, Property_Identifier);
		long Delta_To_Terminator = Search_String_Contents(Contents.data() + Index, ">");

		if(Delta_To_ID < Delta_To_Terminator)
			if (Delta_To_ID != -1)
			{
				Index += Delta_To_ID;

				while (*(Property_Identifier++)) // Since "Search_String_Contents" takes us to the start of the substring, 
												 // we need to just get to the end of it
					Index++;

				while (Contents[Index] != '\"' && Contents[Index] != '/')
					Property += Contents[Index++];
			}
	}

	void Load_XML_Document_Node(const std::string& Contents, long& Index, XML_Document* Parent_Node)
	{
		// Get node type
		long Delta = 1 + Search_String_Contents(Contents.data() + Index, "<"); // Checks for next node

		if (!Delta) // There's nothing more?
			return;		// Just end

		Index += Delta; // Gets next node

		std::string Node_Type = "";

		while (Contents.data()[Index] != ' ' && Contents.data()[Index] != '>')
		{
			Node_Type += Contents.data()[Index];
			Index++;
		}

		std::string ID = "";
		std::string Target = "";
		std::string URL = "";

		Get_XML_Properties(Contents, Index, ID, "id=\"");
		Get_XML_Properties(Contents, Index, Target, "target=\"");
		Get_XML_Properties(Contents, Index, URL, "url=\"");
		if (URL.length() == 0)										// if we haven't found a URL ?
			Get_XML_Properties(Contents, Index, URL, "source=\"");	// grab the 'source' instead as they're analogous

		Index += 1 + Search_String_Contents(Contents.data() + Index, ">"); // Passes node

		Parent_Node->Nodes[Node_Type].push_back(XML_Document());

		XML_Document* Current_Node = &Parent_Node->Nodes[Node_Type].back();

		Current_Node->Id = ID;
		Current_Node->Target = Target;
		Current_Node->URL = URL;

		if (Contents.data()[Index - 2] == '/') // If doesn't have </type> terminator
			return;

		Index += Next_Nonwhitespace(Contents.data() + Index); // Gets first non whitespace character

		if (Contents.data()[Index] == '<') // New node bracket!!
		{
			// Add children in loop!
			// return once </ is reached
			while (Contents.data()[Index + 1] != '/') // Check for termination node bracket </type>
			{
				Load_XML_Document_Node(Contents, Index, Current_Node); 
				// Don't worry about pointers because this adds things in a stack
				// there should be no situation in which this pointer is reallocated somewhere else (due to pushing another element to the vector)

				Index += Search_String_Contents(Contents.data() + Index, "<");
			}
			Index++;
			return; // We've reached </, return to the parent node
		}
		else
		{
			// Add data and return once </ is reached

			//

			while (Contents.data()[Index] != '<') // We can infer that this is the end point
			{
				Current_Node->Data_Array.push_back(""); // Gets new empty string as buffer

				while (!isspace(Contents.data()[Index]) && Contents.data()[Index] != '<')
				{
					Current_Node->Data_Array.back() += Contents.data()[Index];
					Index++;
				}

				Index += Next_Nonwhitespace(Contents.data() + Index);
			}

			Index++;

			return;
		}

		// We'll skip ID stuff for now

		// If we encounter numbers, add data array

		// If we instead encounter a <, add child elements until we encounter <\


	}

	int Load_XML_Document(const char* Filename, XML_Document* Target_Document)
	{
		std::string Contents = Load_File_Contents(Filename);

		long Index = 1 + Search_String_Contents(Contents.data(), ">"); // Skips Collada header

		Load_XML_Document_Node(Contents, Index, Target_Document);

		return 0;
	}
}