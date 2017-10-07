Add common/smmmake to PATH environment variable.

Edit SMMDIR variable defined in command/smmake to its location in your machine.

Enter integrated/script. Run ./configure --profile to collect required information.


For code management:
	Run ./configure --code to generate the makefiles. 
	Run ./run --cm or ./run --ecm, depending on which code management technique you would like to use. cm is the code management technique that tries to find the best mapping, while ecm is the code management technique that removes unnecessary code management functions once a mapping is fined, on top of cm.

For stack management:
	Run ./configure --stack to generate the makefiles. 
	Run ./run --ssm or ./run --esm, depending on which stak management technique you would like to use. ssm is the stack management technique with efficient stack frame management and basic pointer management, while esm is the stack management technique with efficient stack frame management and efficient pointer management.

