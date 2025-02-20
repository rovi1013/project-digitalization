# Development Process

## TODOs

- [x] Which instant messaging protocol should we use?
  - Telegram
- [x] Which modules from RIOT-OS do we need?
  - See [README.md](./README.md) section 'RIOT-OS Modules'
- [x] Create the project architecture.
- [x] Develop the application.
- [x] Present the project.


## Timeline

### 2024-11-25: Architecture

- Submission and presentation of your architecture

### 2025-01-20: Demo

- Present your walking skeleton (incl. demo)

### 2025-02-10: Presentation

- Give a short presentation on your work (live demo?)

### 2025-02-21: Submission

- Final version of the code is in the repository
- You have granted access to me
- Send me your documentation


## Ask the Prof

### Module ``shell_commands``

[Makefile](src/Makefile) module ``shell_commands`` error:

```shell
Error - using unknown modules: shell_commands
make: *** [/home/user/Workspace/project-digitalization/RIOT//Makefile.include:742: ..module-check] Error 1
```

Despite ``make info-modules`` showing that ``shell_commands`` is available. And the module is working in ``Tutorials/task-01`` on the same machine.

**Answer**: Wrong name, correct name is `shell_cmds_default`

### Use of "jsmn" library

The jsmn library (parse JSON) is not included in RIOT OS despite it being mentioned in the [documentation](https://doc.riot-os.org/group__pkg__jsmn.html).

**Answer**: Not a module but a package. Use `USEPKG` instead of `USEMODULE`. Refer to RIOT/tests/ for example
implementations of most features (modules and packages) for the correct and up-to-date implementation.

### Makefile location

Is there any way to use the project structure where the Makefile is not in the same directory as the main.c file?

**Answer**: Yes, use wrapper Makefiles.


## Temperature Sensor DHT20 (NO RIOT DRIVER)
Not supported by RIOT-OS currently, which is why this project is using the CPU temperature.

### PINs
| DHT20 Pin | Function       | nRF52840-DK Pin           |
|-----------|----------------|---------------------------|
| VCC       | Power (3.3V)   | 3V3 on nRF52840-DK        |
| GND       | Ground         | GND                       |
| SDA       | I2C Data Line  | GPIO pin with I2C support |
| SCL       | I2C Clock Line | GPIO pin with I2C support |


## Issue with "double" running on native

Not tested on real board, but probably not an issue

The issue is the double in cpu_temperature.c:
```c++
const double scaled_temp = temp->temperature * scale_factor(temp->scale);
```

Using int instead fixes the problem -> possible solution:
```c++
int scaled_temp = temp->temperature;
printf("[%s] The temperature of %s is %d.%02d °C\n",
       time_str, temp->device_name, scaled_temp / 100, scaled_temp % 100);
```

The problem has something to do with my gcc configuration (probably caused by a recent update).

Although the build itself works without any issues:
```shell
make -C src clean
make[1]: Entering directory '/home/user/Workspace/project-digitalization/src'
make[1]: Leaving directory '/home/user/Workspace/project-digitalization/src'
make -C src all
make[1]: Entering directory '/home/user/Workspace/project-digitalization/src'
Building application "project-digitalization" for "native" with CPU "native".

"make" -C /home/user/Workspace/project-digitalization/RIOT/boards/common/init
"make" -C /home/user/Workspace/project-digitalization/RIOT/boards/native
"make" -C /home/user/Workspace/project-digitalization/RIOT/boards/native/drivers
"make" -C /home/user/Workspace/project-digitalization/RIOT/core
"make" -C /home/user/Workspace/project-digitalization/RIOT/core/lib
"make" -C /home/user/Workspace/project-digitalization/RIOT/cpu/native
"make" -C /home/user/Workspace/project-digitalization/RIOT/cpu/native/cli_eui_provider
"make" -C /home/user/Workspace/project-digitalization/RIOT/cpu/native/netdev_tap
"make" -C /home/user/Workspace/project-digitalization/RIOT/cpu/native/periph
"make" -C /home/user/Workspace/project-digitalization/RIOT/cpu/native/stdio_native
"make" -C /home/user/Workspace/project-digitalization/RIOT/drivers
"make" -C /home/user/Workspace/project-digitalization/RIOT/drivers/netdev
"make" -C /home/user/Workspace/project-digitalization/RIOT/drivers/periph_common
"make" -C /home/user/Workspace/project-digitalization/RIOT/drivers/saul
"make" -C /home/user/Workspace/project-digitalization/RIOT/drivers/saul/init_devs
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/auto_init
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/event
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/evtimer
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/fmt
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/frac
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/iolist
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/isrpipe
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/libc
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/luid
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/application_layer/dhcpv6
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/application_layer/dns
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/application_layer/sock_dns
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/crosslayer/inet_csum
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/gnrc
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/gnrc/application_layer/dhcpv6
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/gnrc/netapi
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/gnrc/netif
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/gnrc/netif/ethernet
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/gnrc/netif/hdr
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/gnrc/netif/init_devs
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/gnrc/netreg
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/gnrc/network_layer/icmpv6
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/gnrc/network_layer/icmpv6/echo
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/gnrc/network_layer/icmpv6/error
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/gnrc/network_layer/ipv6
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/gnrc/network_layer/ipv6/hdr
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/gnrc/network_layer/ipv6/nib
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/gnrc/network_layer/ndp
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/gnrc/pkt
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/gnrc/pktbuf
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/gnrc/pktbuf_static
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/gnrc/pktdump
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/gnrc/sock
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/gnrc/sock/udp
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/gnrc/transport_layer/udp
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/link_layer/eui_provider
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/link_layer/l2util
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/netif
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/netutils
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/network_layer/icmpv6
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/network_layer/ipv6/addr
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/network_layer/ipv6/hdr
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/sock
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/net/transport_layer/udp
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/od
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/phydat
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/posix/inet
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/preprocessor
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/ps
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/random
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/saul_reg
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/shell
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/shell/cmds
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/tsrb
"make" -C /home/user/Workspace/project-digitalization/RIOT/sys/ztimer
"make" -C /home/user/Workspace/project-digitalization/src/utils
/usr/bin/ld: warning: /home/user/Workspace/project-digitalization/src/bin/native/project-digitalization.elf has a LOAD segment with RWX permissions
   text	   data	    bss	    dec	    hex	filename
 219946	   1804	  83208	 304958	  4a73e	/home/user/Workspace/project-digitalization/src/bin/native/project-digitalization.elf
make[1]: Leaving directory '/home/user/Workspace/project-digitalization/src'
make -C src term
make[1]: Entering directory '/home/user/Workspace/project-digitalization/src'
/home/user/Workspace/project-digitalization/RIOT/dist/tools/pyterm/pyterm -ps /home/user/Workspace/project-digitalization/src/bin/native/project-digitalization.elf --process-args tap0 
Twisted not available, please install it if you want to use pyterm's JSON capabilities
Welcome to pyterm!
Type '/exit' to exit.
2025-01-15 17:15:53,556 # RIOT native interrupts/signals initialized.
2025-01-15 17:15:53,556 # RIOT native board initialized.
2025-01-15 17:15:53,556 # RIOT native hardware initialization complete.
2025-01-15 17:15:53,556 # 
2025-01-15 17:15:53,556 # NETOPT_TX_END_IRQ not implemented by driver
2025-01-15 17:15:53,556 # DHCPv6: Selecting interface 6 as upstream
2025-01-15 17:15:53,556 # DHCPv6: Selecting interface 6 as upstream
2025-01-15 17:15:53,556 # main(): This is RIOT! (Version: 2025.01-devel)
2025-01-15 17:15:53,556 # LED control initialized using SAUL
2025-01-15 17:15:53,556 # CPU temperature sensor initialized
2025-01-15 17:15:53,556 # Initializing command control shell

```

Running `cpu-temp` starts the issue:
```shell
Traceback (most recent call last):
  File "/usr/lib/python3.12/cmd.py", line 214, in onecmd
    func = getattr(self, 'do_' + cmd)
           ^^^^^^^^^^^^^^^^^^^^^^^^^^
AttributeError: 'SerCmd' object has no attribute 'do_cpu'

During handling of the above exception, another exception occurred:

Traceback (most recent call last):
  File "/home/user/Workspace/project-digitalization/RIOT/dist/tools/pyterm/pyterm", line 957, in <module>
    myshell.cmdloop("Welcome to pyterm!\nType '/exit' to exit.")
  File "/usr/lib/python3.12/cmd.py", line 138, in cmdloop
    stop = self.onecmd(line)
           ^^^^^^^^^^^^^^^^^
  File "/usr/lib/python3.12/cmd.py", line 204, in onecmd
    return self.emptyline()
           ^^^^^^^^^^^^^^^^
  File "/home/user/Workspace/project-digitalization/RIOT/dist/tools/pyterm/pyterm", line 327, in emptyline
    super().emptyline()
  File "/usr/lib/python3.12/cmd.py", line 227, in emptyline
    return self.onecmd(self.lastcmd)
           ^^^^^^^^^^^^^^^^^^^^^^^^^
  File "/usr/lib/python3.12/cmd.py", line 216, in onecmd
    return self.default(line)
           ^^^^^^^^^^^^^^^^^^
  File "/home/user/Workspace/project-digitalization/RIOT/dist/tools/pyterm/pyterm", line 342, in default
    self._write_char((tok.strip() + "\n").encode("utf-8"))
  File "/home/user/Workspace/project-digitalization/RIOT/dist/tools/pyterm/pyterm", line 706, in _write_char
    input_stream.flush()
BrokenPipeError: [Errno 32] Broken pipe
make[1]: *** [/home/user/Workspace/project-digitalization/src/../RIOT/Makefile.include:868: term] Error 1
make[1]: Leaving directory '/home/user/Workspace/project-digitalization/src'
make: *** [Makefile:13: term] Error 2
```

### Getting to the bottom of it

Get the PID (process id) of the application (has to be running):
```shell
ps aux | grep project
# "project" is the partial name search for the process
# The number in the second column of the output is the PID
# USER PID %CPU %MEM VSZ RSS TTY STAT START TIME COMMAND
```

Use `strace` to get the "output" of the process by id:
```shell
strace -p <process_id>
```

Here I get this output:
```shell
strace: Process <process_id> attached
[ Process PID=<process_id> runs in 32 bit mode. ]
read(0, 

"c", 1)                         = 1
write(1, "c", 1)                        = 1
read(0, "p", 1)                         = 1
write(1, "p", 1)                        = 1
read(0, "u", 1)                         = 1
write(1, "u", 1)                        = 1
read(0, "-", 1)                         = 1
write(1, "-", 1)                        = 1
read(0, "t", 1)                         = 1
write(1, "t", 1)                        = 1
read(0, "e", 1)                         = 1
write(1, "e", 1)                        = 1
read(0, "m", 1)                         = 1
write(1, "m", 1)                        = 1
read(0, "p", 1)                         = 1
write(1, "p", 1)                        = 1
read(0, "\n", 1)                        = 1
write(1, "\r", 1)                       = 1
write(1, "\n", 1)                       = 1
write(1, "TEST 4", 6)                   = 6
write(1, "\n", 1)                       = 1
write(1, "CPU TEMP READING STEP 1, STATUS:"..., 38) = 38
--- SIGFPE {si_signo=SIGFPE, si_code=FPE_FLTINV, si_addr=0x8049acd} ---
+++ killed by SIGFPE (core dumped) +++
```

The output indicates that the program is receiving a SIGFPE (Floating-Point Exception) signal, specifically:
* FPE_FLTINV: This means an invalid floating-point operation occurred. This usually happens when:
  * A floating-point operation involves invalid values (e.g., division by zero, NaN, or infinities).
  * Uninitialized memory is being used in a floating-point calculation.

Running through the process with gdb:
```shell
sudo gdb --pid=<process_id>
# Opens gbd console
# Set breakpoint at cpu_temperature_print
b cpu_temperature_print
# Run the app
run
# Start with cpu-temp in program console
cpu-temp
# Go though the execution
next
```

The output of this looks like this:
```shell
GNU gdb (Ubuntu 15.0.50.20240403-0ubuntu1) 15.0.50.20240403-git
Copyright (C) 2024 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "x86_64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<https://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word".
Attaching to process 312298
Reading symbols from /home/user/Workspace/project-digitalization/src/bin/native/project-digitalization.elf...
Reading symbols from /lib/i386-linux-gnu/libm.so.6...
Reading symbols from /usr/lib/debug/.build-id/96/0a8f17cd87cac5c4c57a696f6ab6abc8cd4c5a.debug...
Reading symbols from /lib/i386-linux-gnu/libc.so.6...
Reading symbols from /usr/lib/debug/.build-id/ec/e90bd35626def53dbc8fa6ec5935e7e697fc8b.debug...
Reading symbols from /lib/ld-linux.so.2...
Reading symbols from /usr/lib/debug/.build-id/ce/cd5495e91804e256284a3ea30f35171ac4a1e1.debug...
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
0xf777b579 in __kernel_vsyscall ()
(gdb) stack
Undefined command: "stack".  Try "help".
(gdb) help
List of classes of commands:

aliases -- User-defined aliases of other commands.
breakpoints -- Making program stop at certain points.
data -- Examining data.
files -- Specifying and examining files.
internals -- Maintenance commands.
obscure -- Obscure features.
running -- Running the program.
stack -- Examining the stack.
status -- Status inquiries.
support -- Support facilities.
text-user-interface -- TUI is the GDB text based interface.
tracepoints -- Tracing of program execution without stopping the program.
user-defined -- User-defined commands.

Type "help" followed by a class name for a list of commands in that class.
Type "help all" for the list of all commands.
Type "help" followed by command name for full documentation.
Type "apropos word" to search for commands related to "word".
Type "apropos -v word" for full documentation of commands related to "word".
Command name abbreviations are allowed if unambiguous.
(gdb) b cpu_temperature_print
Breakpoint 1 at 0x8049a0e: file /home/user/Workspace/project-digitalization/src/cpu_temperature.c, line 72.
(gdb) run
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/user/Workspace/project-digitalization/src/bin/native/project-digitalization.elf 
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
RIOT native interrupts/signals initialized.
RIOT native board initialized.
RIOT native hardware initialization complete.

DHCPv6: No upstream interface found!
main(): This is RIOT! (Version: 2025.01-devel)
LED control initialized using SAUL
CPU temperature sensor initialized
Initializing command control shell
> next
next
shell: command not found: next
> cpu-temp
cpu-temp

Breakpoint 1, cpu_temperature_print (temp=0x8084dc0 <main_stack+11264>) at /home/user/Workspace/project-digitalization/src/cpu_temperature.c:72
72	void cpu_temperature_print(const cpu_temperature_t *temp) {
(gdb) next
74	    format_timestamp(temp->timestamp, time_str, sizeof(time_str));
(gdb) next
75	    if (temp->status == 0) {
(gdb) next
76	        puts("TEST 4");
(gdb) next
TEST 4
78	        printf("CPU TEMP READING STEP 1, TEMP: %d, SCALE: %hhd\n", temp->temperature, temp->scale);
(gdb) next
CPU TEMP READING STEP 1, TEMP: 2500, SCALE: -2
79	        const double scaled_temp = temp->temperature * 0.01; //scale_factor(temp->scale);
(gdb) next
80	        printf("CPU TEMP READING STEP 1, STATUS: %f\n", scaled_temp);
(gdb) next
CPU TEMP READING STEP 1, STATUS: 25.000000
81	        puts("TEST 5");
(gdb) next
TEST 5
83	        printf("[%s] The temperature of %s is %.2f°C\n",
(gdb) next
[00:00:00] The temperature of Mock-Temperature-Sensor is 25.00°C
85	        puts("TEST 6");
(gdb) next
TEST 6
cpu_temp_control (argc=1, argv=0x8084e10 <main_stack+11344>) at /home/user/Workspace/project-digitalization/src/cmd_control.c:40
40	    return 0;
(gdb) next
shell_run_once (shell_commands=0x807fa80 <cmd_control_shell_commands>, line_buf=0x8084f0c <main_stack+11596> "cpu-temp", len=128) at /home/user/Workspace/project-digitalization/RIOT/sys/shell/shell.c:531
531	                break;
(gdb) 
534	        print_prompt();
(gdb) 
> 504	        int res = readline(line_buf, len);
(gdb) 


517	        switch (res) {
(gdb) 
530	                shell_handle_input_line(shell_commands, line_buf);
(gdb) 
531	                break;
(gdb) 
534	        print_prompt();
(gdb) 
> 504	        int res = readline(line_buf, len);
(gdb) 


517	        switch (res) {
(gdb) 
530	                shell_handle_input_line(shell_commands, line_buf);
(gdb) 
531	                break;
(gdb) 
534	        print_prompt();
(gdb) 
> 504	        int res = readline(line_buf, len);
(gdb) 


517	        switch (res) {
(gdb) 
530	                shell_handle_input_line(shell_commands, line_buf);
(gdb) Quit
(gdb) exit
A debugging session is active.

	Inferior 1 [process 315019] will be killed.

Quit anyway? (y or n) y

```

## CoAP Request Issue

Using gdb debugging, the issue seems to come from gcoap.h:
```shell
91	    if (gcoap_req_send(coap_buffer_uint8, pkt->payload_len, &remote, NULL, NULL, (void *)request, GCOAP_SOCKET_TYPE_UDP) < 0) {
(gdb) step
gcoap_req_send (buf=0x80863a0 <coap_buffer_uint8> "", len=274, remote=0x808b2a4 <main_stack+10532>, local=0x0, resp_handler=0x0, context=0x808b499 <main_stack+11033>, tl_type=GCOAP_SOCKET_TYPE_UDP)
    at /home/user/Workspace/project-digitalization/RIOT/sys/net/application_layer/gcoap/gcoap.c:1753
1753	{
(gdb) n
1754	    gcoap_socket_t socket = { 0 };
(gdb) n
1756	    unsigned msg_type  = (*buf & 0x30) >> 4;
(gdb) n
1761	    assert(remote != NULL);
(gdb) n
1763	    res = _tl_init_coap_socket(&socket, tl_type);
(gdb) n
1764	    if (res < 0) {
(gdb) n
1769	    if ((resp_handler != NULL) || (msg_type == COAP_TYPE_CON)) {
(gdb) n
1770	        mutex_lock(&_coap_state.lock);
(gdb) n
1772	        for (int i = 0; i < CONFIG_GCOAP_REQ_WAITING_MAX; i++) {
(gdb) n
1773	            if (_coap_state.open_reqs[i].state == GCOAP_MEMO_UNUSED) {
(gdb) n
1774	                memo = &_coap_state.open_reqs[i];
(gdb) n
1775	                memo->state = GCOAP_MEMO_WAIT;
(gdb) n
1785	        memo->resp_handler = resp_handler;
(gdb) n
1786	        memo->context = context;
(gdb) n
1787	        memcpy(&memo->remote_ep, remote, sizeof(sock_udp_ep_t));
(gdb) n
1788	        memo->socket = socket;
(gdb) n
1802	        switch (msg_type) {
(gdb) n
1806	            if (len > CONFIG_GCOAP_PDU_BUF_SIZE) {
(gdb) n
1808	                memo->state = GCOAP_MEMO_UNUSED;
(gdb) n
1809	                mutex_unlock(&_coap_state.lock);
(gdb) n
1810	                return -EINVAL;
(gdb) n
coap_send_control (argc=2, argv=0x808b5d0 <main_stack+11344>) at /home/user/Workspace/project-digitalization/src/cmd_control.c:67
67	    handle_error(__func__, res);
(gdb) 
```

So the issue is the size of the message, which by default is limited by RIOT to `CONFIG_GCOAP_PDU_BUF_SIZE`, which can be found with:
```shell
grep CONFIG_GCOAP_PDU_BUF_SIZE $(find RIOT/ -name "*.h")
# Results:
#RIOT/sys/include/net/gcoap.h: * gcoap_req_init(&pdu, buf, CONFIG_GCOAP_PDU_BUF_SIZE, COAP_METHOD_GET, NULL);
#RIOT/sys/include/net/gcoap/dns.h: * @ref CONFIG_GCOAP_PDU_BUF_SIZE and must be a power
#RIOT/sys/include/net/gcoap.h:#ifndef CONFIG_GCOAP_PDU_BUF_SIZE
#RIOT/sys/include/net/gcoap.h:#define CONFIG_GCOAP_PDU_BUF_SIZE      (128)
```

So CONFIG_GCOAP_PDU_BUF_SIZE is 128, which is smaller than COAP_BUF_SIZE which is 287.

### Solution: Increase size of CONFIG_GCOAP_PDU_BUF_SIZE

In Makefile:
```makefile
CFLAGS += -DCONFIG_GCOAP_PDU_BUF_SIZE=512
```


