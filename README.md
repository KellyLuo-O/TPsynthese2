# TPsynthese2

tftp client 

- to get a file from the server 
	./gettftp host port file
	- the file is downloaded in the folder where this code is 
	
- to send a file in the server 
	./puttftp host port file 
	- the file is downloaded in the server 
	- it kinda works with multiple packet file (file over 512 bytes)
	- it adds some caracteres at the end of the file for some reason 
	- strangely it doesn't work with the image file 
	
	

documentation that helped building tftp client :

https://www.infraspec.dev/blog/tftp-overview/#:~:text=RRQ%20(%20Read%20Request%20),mode%20(binary%20or%20ASCII).

https://man7.org/linux/man-pages/man3/getaddrinfo.3.html
https://linux.die.net/man/3/send
https://man7.org/linux/man-pages/man2/connect.2.html
https://man7.org/linux/man-pages/man2/open.2.html
https://linux.die.net/man/2/sendto


