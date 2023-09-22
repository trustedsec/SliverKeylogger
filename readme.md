#  Sliver Keylogger

This extension to sliver was written mostly as a learning exercise ![image](https://user-images.githubusercontent.com/3172440/174394786-94255ada-2263-4b0b-8076-348b852b2546.png)
 into how sliver handles extensions.

I would like to thank the entire [Sliver](https://github.com/BishopFox/sliver) team for there work on this public implant framework.
I would especially like to thank [@rkervell](https://twitter.com/rkervell) for answering many of my questions related to how sliver handles it extensions

## how does this work

Basically it uses [raw input](https://docs.microsoft.com/en-us/windows/win32/inputdev/raw-input).  This has advantages and disadvantages to other common keylogging methods.

I like it because it doesn't require injecting into other binaries, nor does it require polling a given call frequently and potentially missing results.

This code is some of my own work and some of a mash of examples found around the internet.

## how to build it

open the solution file using visual studio 2019, and build the x86 and x64 releases

## how to load / install it

Until this is potentially included in the [armory](https://github.com/sliverarmory) it can be built on a windows system. then the resulting $(solutiondir)\bin folder can be placed on the sliver client
and `extensions install <path to folder>` can be run to install the extension.  you may need to restart or also run `extensions load <path to folder>`

## usage

raw_keylogger \<cmdid\>

the following cmdid's are valid
```
0 = stop
1 = start
2 = get keystrokes
```

if everything goes well it will look like this
![Screen Shot 2022-06-17 at 3 05 09 PM](https://user-images.githubusercontent.com/3172440/174394494-95aebc3f-3250-4374-9feb-33b5286cf2a7.png)

## lessons learned

Sliver's api has the following two main parts
```cpp
typedef int (*goCallback)(const char*, int);

extern "C" {
	__declspec(dllexport) int __cdecl entrypoint(char* argsBuffer, uint32_t bufferSize, goCallback callback);
}
```

the implant will call the "entrypoint" as defined in the .json file. it points to your entrypoint function (doesn't have to have that name, just follow the signature)

goCallback is a function for returning output to go.   its called like `callback(string, string_length)`

When coding the extension for now the "name" and "command_name" must match, otherwise the implant will reload your extension on every call.


## Utilizing the `parse_keylogger.py` Script

To process and interpret the keylogger output, you can use the `parse_keylogger.py` script located in the `scripts/` directory.

### How to Use:

1. Ensure you have the necessary Python environment set up and active.
2. Navigate to the `scripts/` directory.
3. You can run the script in two ways:
   - By passing the path to a log file:
     ```bash
     python parse_keylogger.py --file path_to_log_file.txt
     ```
   - By directly passing the log content as a string:
     ```bash
     python parse_keylogger.py --string "your_log_content_here"
     ```

The script will process the log content, interpret special keystroke sequences, and print the human-readable text.

Note: The script currently handles special sequences like `[bs]` for backspaces and `[enter]` for newline entries. Other sequences such as arrow keys are recognized but not fully interpreted for cursor movement.


As of this writing non-BOF extensions don't support strongly typed arguments, so everything provided on the cli after the command name will be sent down as a string.  you have to handle it from there on the extension's native code side.
