# Access-Control-Logging			
Developed by Georgios Michail Siatras and Andreas Karogiannis	        

TUC: Security of Systems and Services		            

Proffessor: Sotirios Ioannidis		        


***Access Control Logging tool***			
The tool was created following the given instructions.		
**Output format:**		
//Log1		
UID: UID's	
File Name: file_name.txt		
Date: yy-mm-dd		
Time: hh:mm:ss 				
Access Type: (0=> creation, 1=> open, 2=> write)			
Is-action-denied flag: (0=>not denied, 1=>denied)			
File fingerprint: md5  				
//newline			
//Log2 ....				
**Methods:**			
*recover_filename:*			
Inputs: stream (file)			
Recover filename from path like(home/desktop/filename => filaname) 	for fwrite.			

*calculate_file_md5:*				
Inputs: original_fopen_ret (file)			
Calculate the file fingerprint using md5  				


***Access Control Log Monitoring tool***				
The tool was created following the given instructions.				
**Methods:**				
*printMalUsers:*			
Global variables:			
struct User : uid (int)			
    		  denied_count (int)			
    		  times_modified (int)				
              fingerprint (char)			
MAX_USERS: 5 We can handle 5 different users UID's (can be set to higher if needed).				

We open the log file and start reading line by line, when we find UID we store it the Users array. After when we find the is action_denied flag raised we find the last uid we encounter (its above) and we raise the denied_count.				

*printUsers:*			
Inputs: filename_given (char)				

We open the file and start reading line by line starting from the bottom. First we read the fingerprints, store them in the temporary variable prev_fingerprint. Then we read the file name, if the file name matches we store the temp fingerprint to the cur_fingerprint var. And we raise the flag for file found. With the flag raised we read until we find UID, we check if it is a new UID encounter and then we store it to the uids array, also store the fingerprint. If it is not a new entry we check if the cur_fingerprint is the same with the last stored in the array. If different we store it and raise the times_modified flag. After parsing through all the file we have an array with all the users that tried to access the file, with the times they modified it.			
