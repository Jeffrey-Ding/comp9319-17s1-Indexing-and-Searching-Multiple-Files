# comp9319-17s1-Indexing-and-Searching-Multiple-Files
C++ code for Assignment 3 of COMP9319-Web Data Compression &amp; Search



Specification:

Your task in this assignment is to create a search program that indexes and searches multiple ASCII files in a case insensitive manner. For simplicity, you may assume these files are English documents. 


Your C/C++ program, called  a3search, accepts the path to a destination folder that contains the target files; the path to a folder of index files; an optional argument -c cnum specifies the search is a concept search where cnum is a decimal number between 0 and 1 (and can be equal to 0 or 1); and one to five quoted query strings (i.e., search terms) as commandline input arguments.  A search term may include English alphabets (i.e., you do not need to consider spaces, digits and symbols). Each search term is at least 3 characters and can be up to 256 characters. Using the given query strings, it will search on all the target files, and output (to standard output) all the files (the filenames only) that contain ALL input query strings (i.e., a boolean AND search), ranked on how frequently the search term has appeared in the files in descending order.  



For multiple search terms, it will rank the results based on the average frequency per search term for each file. If their frequencies are the same, the filenames will be output in ascending order (lexicographically). When concept search is involved, we assume the frequency of each concept match is cnum. Hence, when cnum is equal to 1, its ranking will be treated exactly the same as the one of a keyword match. When cnum is equal to 0, the contribution of the frequency counts from the concept matches will be disregarded during ranking (i.e., they will be ranked below those keyword matches). 


The total size of all the files (maximum 2000 files) inside a destination folder will be  max 200MB. The total size of all your index files inside your index folder cannot be more than 1MB or the same size of all the given files (whichever is larger). If your index file is larger than this maximum, you will  receive zero points for the tests that using that folder of given files. You may assume that the index folder and its files inside will not be deleted during all the tests for a given destination folder, and the name of the index folder will not be changed for that given destination folder. Therefore,  to save time, you only need to generate the index file when it does not exist yet.  


Your search may ignore stop words and shall include the stemming process. External library may be used for this process (and if you do, please submit it as part of your solution). You shall not perform just mechanical substring search such as Boyer Moore over the original text as this will result in lots of meaningless matches.  To make the assignment easier, we assume that there is no subfolder inside the destination folder.  Your output shall be one line  (ending with a '\n') for each filename.  No filename will be output more than once. 




make command is used to compile solution.  A makefile is provided.  The solution will be compiled and run on a typical CSE Linux machine.


Example


	Suppose there are five files in ~MyAccount/XYZ/:  
	file1.txt: Apple investors brace for first decline in iPhone sales
	file2.txt: Earnings deluge threatens to overwhelm investors
	file3.txt: Apple Watch from Apple disappoints some investors
	file4.txt: People are buying iPhone
	file5.txt: Invest in Apple shares because of their apps?
	file6.txt: Go Apple! Apple! Apple! Go iPhone!
 	
	First example: 
	%wagner> a3search ~MyAccount/XYZ dummy.idx "apple"
	file6.txt
	file3.txt
	file1.txt
	file5.txt
	%wagner> 



	Another example: 
	%wagner> a3search ~MyAccount/XYZ dummy.idx "INvestor"
	file1.txt
	file2.txt
	file3.txt
	%wagner> 

	Yet another example: 
	%wagner> a3search ~MyAccount/XYZ dummy.idx "apple" "investor"
	file3.txt
	file1.txt
	%wagner> 

	%wagner> a3search ~MyAccount/XYZ dummy.idx "apple" "APP"
	file5.txt
	
	%wagner> a3search ~MyAccount/XYZ dummy.idx "apple" "APP" "sales"
	%wagner> 



PERFORMANCE:

Runtime memory is assumed to be always less than 32MB. Runtime memory consumption will be measured by valgrind massif with the option  --pages-as-heap=yes, i.e., all the memory used by your program will be measured.  Any solution that violates this memory requirement will receive zero points for that query test. Any solution that runs for more than 300 seconds on a machine with similar specification  as wagner for the first query on a given destination folder will be killed, and will receive zero points for the queries for that folder.   After that any solution that runs for more than 5 seconds for any one of the subsequent queries on that folder will be killed,  and will receive zero points for that query test.
