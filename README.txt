# ===================================================================================== #
# The README.txt is written by Jiarui Ding (z5045636) for COMP9319 Assignment 3 - 17s1. #
# ===================================================================================== #


1. Building inverted index:
My program firstly does linear scan over all the files inside the destination folder, taking only consecutive alphabet characters (with length >= 3) as a valid token.
After stop words filtering and stemming, I used the term, the current file, as well as the term frequency in the file to build inverted index in memory. I also assigned a unique offset to every filename, in order to save space for my inverted index. 
The inverted index is lexicographically sorted based on the term, and for the same term, the posting is sorted based on the offset of its filename in ascending order.

For example, my inverted index looks like:
apple:1 12;2 15;
banana:2 3;3 10;

My mapping between filename and offset looks like:
1/file1.txt
2/file2.txt
3/file3.txt

which means term "apple" occurs 12 times in file1.txt, occurs 15 times in file2.txt; term "banana" occurs 3 times in file2.txt, occurs 10 times in file3.txt.

To ensure my runtime memory is always less than 32MB, for every 14MB of original files¡¯ size I read, I write my inverted index in memory to a new external index file. After all inverted indexes have been written to the index folder, I finally write the mapping between filename and offset to the index folder as well.

[* I used porter2_stemmer library to complete stemming process, which contains 2 files - porter2_stemmer.cpp and porter2_stemmer.h. They are packed in porter2_stemmer.tar file.]



2. Searching:
a3search accepts 1-5 search terms, and only files which contain all the search terms should be valid output.
My strategy is to firstly sort the five search terms in lexicographic order. I linear scan all the index files for once to find the matched postings, and merge these postings from different index files for the same term. The advantage of both lexicographically sorted search terms and inverted index is that I can only search the index for once to get all the up to five matched postings.

For example, If the search terms are ¡±apples¡±, ¡°banana¡±, and matched postings in my Index_A are:
apple:1 12;2 15;
banana:2 3;3 10;

Matched postings in my Index_B are:
apple:4 4;6 12;
banana:6 10;

Then the merged postings should be:
apple: 1 12;2 15;4 4;6 12;
banana: 2 3;3 10;6 10;

Based on the merged postings, the intersection of all the terms¡¯ postings should be generated in the form of <File ID : Total term frequency>. 
Using the previous example, we should get:
2:18
6:22.

Which means file ID 2 is matched with the sum of the matched terms¡¯ count 18, and file ID 6 is matched with the sum of the matched terms¡¯ count 22.

Lastly, I get the mapping between filename and offset, and assign the filename back to the matched results, based on which I rank and output the results for each file with the decreasing frequency of the search terms. If files have same term frequencies, the output order is lexicographically ascending of filenames.

Using the above example, the final output should be:
file6.txt
file2.txt


