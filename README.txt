Add common/smmmake to PATH environment variable.

Edit SMMDIR variable defined in command/smmake to its location in your machine.

Enter experiments/run. Execute the following command to collect required information for SPM management.
	./configure --profile

For code management:
	Generate makefiles.
		$ ./configure --code
	Run benchmarks. In the below commands, cm is the code management technique that tries to find the best mapping, while ecm is the code management technique that removes unnecessary code management functions once a mapping is fined, on top of cm.
		$ ./run --cm
		$ ./run --ecm

For stack management:
	Generate the makefiles.
		./configure --stack
	Execute applications. In the below commands, ssm is the stack management technique with efficient stack frame management and basic pointer management, while esm is the stack management technique with efficient stack frame management and efficient pointer management.
		$ ./run --ssm or
		$ ./run --esm
