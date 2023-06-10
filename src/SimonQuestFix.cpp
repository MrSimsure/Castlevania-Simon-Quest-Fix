#include <iostream>
#include <string>
#include <Windows.h>

using namespace std;


int speedDialogs = 1;
int speedDay = 3;
const size_t byteSize = sizeof(BYTE);


/* Try to convert a string to an int, if the conversion cannot be performed, use the default value */
int strignToInt(string value, int defaultValue=-1)
{
	try
	{
		return stoi(value);
	}
	catch(invalid_argument e)
	{
		return defaultValue;
	}
}


/* Parse the program arguments and set the global variables */
void ParseArguments(int argc, char* argv[])
{
	if(argc < 2)
		return;

	for(int i=1; i<argc; i++)
	{
		string curr = argv[i];

		// If the option '-dia' is present set the 'speedDialogs' accordingly
		if(curr == "-dia")
		{
			string value = argv[i + 1];
			speedDialogs = strignToInt(value, speedDialogs);

			i += 1;
		}

		// If the option '-day' is present set the 'speedDay' accordingly
		if(curr == "-day")
		{
			string value = argv[i + 1];
			speedDay = strignToInt(value, speedDay);
			i += 1;
		}
	}
}


/* Check if a passed byte is the first value of a specified sequence, 
 * in that case keep looking the next bytes and return true if all the sequence is found */
bool analyzeBytes(BYTE currentData, BYTE byteSequence[], int sequenceSize, unsigned char* memoryPosition)
{
	if(currentData == byteSequence[0])
	{
		for(int indexCheck=1; indexCheck<sequenceSize; indexCheck++)
		{
			BYTE currentCheckData;
			memcpy(&currentCheckData, memoryPosition+indexCheck, byteSize);

			// If a value in the sequence is not correct, break the search loop
			if(currentCheckData != byteSequence[indexCheck])
				return false;

			// If every byte of the sequence is correct, we found the byte!
			if(indexCheck == sequenceSize-1)
				return true;
		}
	}

	return false;
}


/* Given a process handler, scan it's memory looking for the correct sequence of bytes */
bool ProcessScan(HANDLE processHandler)
{
	const unsigned CHUNK_SIZE = 0x050000;
	const unsigned MAX_ADDRESS = 0x7FFFFFFF;
	unsigned char buffer[CHUNK_SIZE];
	bool checkDialog = false;
	bool checkDay = false;

	const int dialogSequenceSize = 6; 			
	BYTE dialogByteSequence[dialogSequenceSize] = { 240, 3, 198, 123, 96, 169 };

	const int daySequenceSize = 5;
	BYTE dayByteSequence[daySequenceSize] = { 101, 135, 133, 135, 169 };


	for(int indexChunk=0; indexChunk<MAX_ADDRESS; indexChunk+=CHUNK_SIZE)
	{
		if(ReadProcessMemory(processHandler, reinterpret_cast<LPVOID>(indexChunk), buffer, sizeof(buffer), NULL))
		{
			for(int indexMem=0; indexMem<=CHUNK_SIZE-sizeof(int); ++indexMem)
			{
				BYTE currentData;
				memcpy(&currentData, buffer+indexMem, byteSize);

				// Look for the possible sequence of bytes that precedes the dialog speed byte				
				if(analyzeBytes(currentData, dialogByteSequence, dialogSequenceSize, buffer+indexMem))
				{
					BYTE speedDialogsByte = speedDialogs;
					WriteProcessMemory(processHandler, reinterpret_cast<LPVOID>(indexChunk+indexMem+6), (LPCVOID)(&speedDialogsByte), sizeof(speedDialogsByte), NULL);
					printf("Dialog speed changed to %d\n", speedDialogs);
					checkDialog = true;
				}

				// Look for the possible sequence of bytes that precedes the day/night speed byte
				if(analyzeBytes(currentData, dayByteSequence, daySequenceSize, buffer+indexMem))
				{
					BYTE speedDayByte = speedDay;
					WriteProcessMemory(processHandler, reinterpret_cast<LPVOID>(indexChunk+indexMem+5), (LPCVOID)(&speedDayByte), sizeof(speedDayByte), NULL);
					printf("Day/Night speed changed to %d\n", speedDay);
					checkDay = true;
				}

				// Check if everything is done
				if(checkDialog == true && checkDay == true)
				{
					printf("\nAll done.\n");
					indexChunk = MAX_ADDRESS;
					break;
				}
			}
		}
	}


	if (checkDialog == false && checkDay == false)
		return false;
	else
		return true;
}



/* Program start */
int main(int argc, char* argv[])
{
	ParseArguments(argc, argv);

	printf("Looking for the game process...\n");
	HWND windowHandler = FindWindowA(NULL, "Castlevania Anniversary Collection");

	if(windowHandler == NULL)
	{
		printf("Can't find Castlevania Anniversary Edition from Steam, start the game and retry.\n");
		system("PAUSE");
		return -1;
	}
	else
	{
		DWORD processId;
		GetWindowThreadProcessId(windowHandler, &processId);
		HANDLE processHandler = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

		if(processHandler == NULL)
		{
			printf("Error: cannot find the game process.\n");
			system("PAUSE");
			return -1;
		}

		printf("Process found, looking for the correct byte sequence...\n");
		bool result = ProcessScan(processHandler);
		
		if(result == false)
			printf("Castlevania Anniversary Edition has been found, but not Simon's quest, start it and retry.\n");
		
	}

	system("PAUSE");
}

