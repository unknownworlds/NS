//
// (http://planethalflife.com/botman/)
//
// exports.c
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>


#define DOS_SIGNATURE   0x5A4D       /* MZ */
#define NT_SIGNATURE    0x00004550   /* PE00 */


// globals
WORD *p_Ordinals = NULL;
DWORD *p_Names = NULL;
DWORD *p_Functions = NULL;
int num_ordinals;


typedef struct {                        // DOS .EXE header
    WORD   e_magic;                     // Magic number
    WORD   e_cblp;                      // Bytes on last page of file
    WORD   e_cp;                        // Pages in file
    WORD   e_crlc;                      // Relocations
    WORD   e_cparhdr;                   // Size of header in paragraphs
    WORD   e_minalloc;                  // Minimum extra paragraphs needed
    WORD   e_maxalloc;                  // Maximum extra paragraphs needed
    WORD   e_ss;                        // Initial (relative) SS value
    WORD   e_sp;                        // Initial SP value
    WORD   e_csum;                      // Checksum
    WORD   e_ip;                        // Initial IP value
    WORD   e_cs;                        // Initial (relative) CS value
    WORD   e_lfarlc;                    // File address of relocation table
    WORD   e_ovno;                      // Overlay number
    WORD   e_res[4];                    // Reserved words
    WORD   e_oemid;                     // OEM identifier (for e_oeminfo)
    WORD   e_oeminfo;                   // OEM information; e_oemid specific
    WORD   e_res2[10];                  // Reserved words
    LONG   e_lfanew;                    // File address of new exe header
  } DOS_HEADER, *P_DOS_HEADER;

typedef struct {
    WORD    Machine;
    WORD    NumberOfSections;
    DWORD   TimeDateStamp;
    DWORD   PointerToSymbolTable;
    DWORD   NumberOfSymbols;
    WORD    SizeOfOptionalHeader;
    WORD    Characteristics;
} PE_HEADER, *P_PE_HEADER;

#define SIZEOF_SHORT_NAME              8

typedef struct {
    BYTE    Name[SIZEOF_SHORT_NAME];
    union {
            DWORD   PhysicalAddress;
            DWORD   VirtualSize;
    } Misc;
    DWORD   VirtualAddress;
    DWORD   SizeOfRawData;
    DWORD   PointerToRawData;
    DWORD   PointerToRelocations;
    DWORD   PointerToLinenumbers;
    WORD    NumberOfRelocations;
    WORD    NumberOfLinenumbers;
    DWORD   Characteristics;
} SECTION_HEADER, *P_SECTION_HEADER;

typedef struct {
    DWORD   VirtualAddress;
    DWORD   Size;
} DATA_DIRECTORY, *P_DATA_DIRECTORY;

#define NUMBEROF_DIRECTORY_ENTRIES    16

typedef struct {
    WORD    Magic;
    BYTE    MajorLinkerVersion;
    BYTE    MinorLinkerVersion;
    DWORD   SizeOfCode;
    DWORD   SizeOfInitializedData;
    DWORD   SizeOfUninitializedData;
    DWORD   AddressOfEntryPoint;
    DWORD   BaseOfCode;
    DWORD   BaseOfData;
    DWORD   ImageBase;
    DWORD   SectionAlignment;
    DWORD   FileAlignment;
    WORD    MajorOperatingSystemVersion;
    WORD    MinorOperatingSystemVersion;
    WORD    MajorImageVersion;
    WORD    MinorImageVersion;
    WORD    MajorSubsystemVersion;
    WORD    MinorSubsystemVersion;
    DWORD   Win32VersionValue;
    DWORD   SizeOfImage;
    DWORD   SizeOfHeaders;
    DWORD   CheckSum;
    WORD    Subsystem;
    WORD    DllCharacteristics;
    DWORD   SizeOfStackReserve;
    DWORD   SizeOfStackCommit;
    DWORD   SizeOfHeapReserve;
    DWORD   SizeOfHeapCommit;
    DWORD   LoaderFlags;
    DWORD   NumberOfRvaAndSizes;
    DATA_DIRECTORY DataDirectory[NUMBEROF_DIRECTORY_ENTRIES];
} OPTIONAL_HEADER, *P_OPTIONAL_HEADER;

typedef struct {
    DWORD   Characteristics;
    DWORD   TimeDateStamp;
    WORD    MajorVersion;
    WORD    MinorVersion;
    DWORD   Name;
    DWORD   Base;
    DWORD   NumberOfFunctions;
    DWORD   NumberOfNames;
    DWORD   AddressOfFunctions;     // RVA from base of image
    DWORD   AddressOfNames;         // RVA from base of image
    DWORD   AddressOfNameOrdinals;  // RVA from base of image
} EXPORT_DIRECTORY, *P_EXPORT_DIRECTORY;


void FreeNameFuncGlobals(void)
{
   if (p_Ordinals)
      free(p_Ordinals);
   if (p_Functions)
      free(p_Functions);
   if (p_Names)
      free(p_Names);
}


void FgetString(char *str, FILE *bfp)
{
   char ch;

   while ((ch = fgetc(bfp)) != EOF)
   {
      *str++ = ch;
      if (ch == 0)
         break;
   }
}


int main(int argc, char *argv[])
{
   FILE *bfp;
   char filename[80];
   BOOL extended = FALSE;
   DOS_HEADER dos_header;
   LONG nt_signature;
   PE_HEADER pe_header;
   SECTION_HEADER section_header;
   BOOL edata_found;
   OPTIONAL_HEADER optional_header;
   LONG edata_offset;
   LONG edata_delta;
   EXPORT_DIRECTORY export_directory;
   LONG name_offset;
   LONG ordinal_offset;
   LONG function_offset;
   char function_name[256];
   int i, index;
   BOOL error;
   //char msg[80];


   if (argc < 2)
   {
      printf("usage: exports [-e] filename.dll\n");
      return -1;
   }

   if (argc > 2)
   {
      if (argv[1][0] == '-')
      {
         if (argv[1][1] == 'e')
         {
            strcpy(filename, argv[2]);
            extended = TRUE;
         }
         else
         {
            printf("unknown option \"%s\"\n\n", argv[1]);
            printf("usage: exports [-e] filename.dll\n");
            return -1;
         }
      }
      else
      {
         printf("usage: exports [-e] filename.dll\n");
         return -1;
      }
   }
   else
      strcpy(filename, argv[1]);

   if ((bfp=fopen(filename, "rb")) == NULL)
   {
      printf("DLL file %s not found!", filename);
      return -1;
   }

   if (fread(&dos_header, sizeof(dos_header), 1, bfp) != 1)
   {
      printf("%s is NOT a valid DLL file!", filename);
      return -1;
   }

   if (dos_header.e_magic != DOS_SIGNATURE)
   {
      printf("file does not have a valid DLL signature!");
      return -1;
   }

   if (fseek(bfp, dos_header.e_lfanew, SEEK_SET) == -1)
   {
      printf("error seeking to new exe header!");
      return -1;
   }

   if (fread(&nt_signature, sizeof(nt_signature), 1, bfp) != 1)
   {
      printf("file does not have a valid NT signature!");
      return -1;
   }

   if (nt_signature != NT_SIGNATURE)
   {
      printf("file does not have a valid NT signature!");
      return -1;
   }

   if (fread(&pe_header, sizeof(pe_header), 1, bfp) != 1)
   {
      printf("file does not have a valid PE header!");
      return -1;
   }

   if (pe_header.SizeOfOptionalHeader == 0)
   {
      printf("file does not have an optional header!");
      return -1;
   }

   if (fread(&optional_header, sizeof(optional_header), 1, bfp) != 1)
   {
      printf("file does not have a valid optional header!");
      return -1;
   }

   edata_found = FALSE;

   for (i=0; i < pe_header.NumberOfSections; i++)
   {
      if (fread(&section_header, sizeof(section_header), 1, bfp) != 1)
      {
         printf("error reading section header!");
         return -1;
      }

      if (strcmp((char *)section_header.Name, ".edata") == 0)
      {
         edata_found = TRUE;
         break;
      }
   }

   if (edata_found)
   {
      edata_offset = section_header.PointerToRawData;
      edata_delta = section_header.VirtualAddress - section_header.PointerToRawData; 
   }
   else
   {
      edata_offset = optional_header.DataDirectory[0].VirtualAddress;
      edata_delta = 0L;
   }


   if (fseek(bfp, edata_offset, SEEK_SET) == -1)
   {
      printf("file does not have a valid exports section!");
      return -1;
   }

   if (fread(&export_directory, sizeof(export_directory), 1, bfp) != 1)
   {
      printf("file does not have a valid optional header!");
      return -1;
   }

   num_ordinals = export_directory.NumberOfNames;  // also number of ordinals


   ordinal_offset = export_directory.AddressOfNameOrdinals - edata_delta;

   if (fseek(bfp, ordinal_offset, SEEK_SET) == -1)
   {
      printf("file does not have a valid ordinals section!");
      return -1;
   }

   if ((p_Ordinals = (WORD *)malloc(num_ordinals * sizeof(WORD))) == NULL)
   {
      printf("error allocating memory for ordinals section!");
      return -1;
   }

   if (fread(p_Ordinals, num_ordinals * sizeof(WORD), 1, bfp) != 1)
   {
      FreeNameFuncGlobals();

      printf("error reading ordinals table!");
      return -1;
   }


   function_offset = export_directory.AddressOfFunctions - edata_delta;

   if (fseek(bfp, function_offset, SEEK_SET) == -1)
   {
      FreeNameFuncGlobals();

      printf("file does not have a valid export address section!");
      return -1;
   }

   if ((p_Functions = (DWORD *)malloc(num_ordinals * sizeof(DWORD))) == NULL)
   {
      FreeNameFuncGlobals();

      printf("error allocating memory for export address section!");
      return -1;
   }

   if (fread(p_Functions, num_ordinals * sizeof(DWORD), 1, bfp) != 1)
   {
      FreeNameFuncGlobals();

      printf("error reading export address section!");
      return -1;
   }


   name_offset = export_directory.AddressOfNames - edata_delta;

   if (fseek(bfp, name_offset, SEEK_SET) == -1)
   {
      FreeNameFuncGlobals();

      printf("file does not have a valid names section!");
      return -1;
   }

   if ((p_Names = (DWORD *)malloc(num_ordinals * sizeof(DWORD))) == NULL)
   {
      FreeNameFuncGlobals();

      printf("error allocating memory for names section!");
      return -1;
   }

   if (fread(p_Names, num_ordinals * sizeof(DWORD), 1, bfp) != 1)
   {
      FreeNameFuncGlobals();

      printf("error reading names table!");
      return -1;
   }

   error = FALSE;

   for (i=0; (i < num_ordinals) && (error==FALSE); i++)
   {
      name_offset = p_Names[i] - edata_delta;

      if (name_offset != 0)
      {
         if (fseek(bfp, name_offset, SEEK_SET) == -1)
         {
            printf("error in loading names section!\n");
            error = TRUE;
         }
         else
         {
            FgetString(function_name, bfp);

            if (extended)
            {
               index = p_Ordinals[i];

               printf("ordinal=%3d addr=%08lX name=%s\n",
                   (p_Ordinals[i]+1), p_Functions[index], function_name);
            }
            else
               printf("%s\n", function_name);
         }
      }
   }

   FreeNameFuncGlobals();

   fclose(bfp);

   return 0;
}


