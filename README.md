# bitflipper

This command-line utility flips (turns 1 into 0 and vice versa) a specified amount of randomly chosen bits in any file.

## Installation
Compile `main.c` and execute in your preffered system with arguments listed below

## Usage
### Arguments
- `-n, --amount <number>` set amount of bits to flip in a file. **0 if not provided.**
- `-s, --seed <number>` sets the seed for random generator. Random generator is responsible for picking the bits to flip. **0 if not provided.**
- `-s, --seed time` sets the seed from current system time.
- `-h, --help` shows the help message.
- `-v, --verbose` enables verbose mode.
- `--disk` write changes to the disk immediately as they appear.
- `--ram` make changes in the RAM copy of the file, then rewrite the file once with modified data.
- `--skip <number>` amount of **bytes** from the beginning of the file the bitflipper will not affect.

### Examples
`bitflipper ./image.bmp -n 1000 -stime`

This will flip 1000 bits in the file 'image.bmp' with a different seed each time it's executed.

Interesting behavior can be seen if a command is executed **twice** with the same seed and number of bits:

`bitflipper ./image2.bmp -n 10000 -s 1234`

On the first execution the file is going to get partially or fully corrupted, but on the second run the file is going to return to it's initial state, since the bits flipped are going to be exactly the same as before. This essentially decrypts the file.