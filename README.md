Small operating system for the Raspberry Pi 2 I wrote in summer 2018. It features
the ability to create processes, restrict memory accesses and a functional
terminal using the UART interface. There are also drivers to use the general
IO pins to drive LEDs.

The coding style conforms to the linux kernel coding style.

To run your program, add the source files to the directory `user/`. The function
`int main()` is the main function of the user process. There is an example
user process given in this repository.

To build the OS and install it via network, run the command `make install`.

