//============================================================================
// Name        : a3search.cpp
// Author      : Jiarui Ding (z5045636)
// Version     :
// Copyright   : Your copyright notice
// Description : The following code is written for COMP9319 Assignment3 - 17s1
//============================================================================

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <dirent.h>
#include <string.h>
#include <map>
#include <set>
#include <ctype.h>
#include <cstdlib>
#include <algorithm>
#include <cctype>


// IMPORT PORTER2_STEMMER LIBRARY!
#include <chrono>
#include "porter2_stemmer.h"

using namespace std;



// Stop words set:
set<string> stop_words = {"the", "and", "but", "because", "until", "while", "for", "with", "about", "against", "between", "into",
						  "through", "during", "before", "after", "above", "below", "from", "down", "out", "off", "over",
						  "under", "again", "further", "then", "once", "here", "there", "when", "where", "why", "how", "all",
						  "any", "both", "each", "few", "more", "most", "other", "some", "such", "nor", "not", "only",
						  "own", "same", "than", "too", "very",
						  "least", "now", "never", "whether", "even", "still", "since", "however",
						  "across", "after", "also", "either", "else", "ever", "every",
						  "myself", "our", "ours","ourselves", "you", "your","yours", "yourself", "yourselves",
						  "him", "his", "himself", "she", "her", "hers", "herself", "just",
						  "its", "itself", "they", "them", "their", "theirs", "themselves",
						  "what", "which", "who", "whom", "this", "that", "these", "those",
						  "are", "was", "were", "been", "being", "have", "has", "had", "having", "does", "did", "doing",
						  "will", "would", "shall", "should", "can", "could", "may", "might", "must", "ought",
						  "isn", "aren", "wasn", "weren", "hasn", "haven", "hadn", "doesn", "don", "didn",
						  "won", "wouldn", "shan", "shouldn", "cannot", "couldn", "mustn"};



// Global variables:

// 1. Buffer size of file reader:
int buffer_size = 7*1024*1024;

// 2. Map between file name and file ID:
map<int, string> file_name_index;


// 3. Map of inverted index: < Term : < File_ID : Count > >
map<string, map<int,int>> inverted_index;


// 4 Set a threshold of already read file size; and the size of accumulated already read files
int read_thresh = 14*1024*1024;
int read_size = 0;


// 5. Record a char as index file number
char no_idx_file = 65;

// 6. Map file name to index:
int file_index = 1;


// 7. A map to store matched terms and postings < Term : < File_ID : Count > >
map<string, map<int,int>> matched;




// Functions:

// F1. get file size:
int get_file_size(string file){
	ifstream in(file, ios::in);
	in.seekg(0, ios::end);
    int file_size = in.tellg();
    in.close();
    return file_size;
}


// F2: Tokenize and process symbol, put term into map:
void symbol_process(char* buffer, int buffer_size, int file_index){

	// Maintain a term string and its length:
	string term = "";
	int term_length = 0;

	// Go through the whole file:
	for (int i = 0; i < buffer_size; i++) {
		char symbol = buffer[i];

		// Step 0: Accumulate the term until a non-alphabet symbol is met:

		// If the character matches: [a-zA-Z]
		if (isalpha(symbol)){
			term += tolower(symbol);
			term_length++;

			continue;
		}

		// Else if symbol is other than alphabet:

		// Step 1: If length of term is shorter than 3, discard it:
		if (term_length < 3){
			term = "";
			term_length = 0;
			continue;
		}


		// Step 2: If the term is in stop_words set, discard it:
		auto search = stop_words.find(term);

		if(search != stop_words.end()) {
			term = "";
			term_length = 0;
			continue;
		}


		// Step 3: Term stemming:
		Porter2Stemmer::stem(term);
		//cout << file_index << ':' << term << endl;


		// Step 4: Put the document id of the term into map, as well as its count:

		map<string, map<int,int>>::iterator it = inverted_index.find(term);
		if (it != inverted_index.end()){

			map<int,int>::iterator it1 = it->second.find(file_index);
			if (it1 != it->second.end()){
				inverted_index[term][file_index] += 1;
			}

			else{
				inverted_index[term][file_index] = 1;
			}


		}
		else{
			inverted_index[term][file_index] = 1;
		}


		// Final step: Re-initialize term and its length
		term = "";
		term_length = 0;


	}

}



// F3: Read file into buffer:
void read_file(ifstream& fin, int current_time, int total_times, int file_size, int file_index){

	// jump to the current position in file:
	fin.seekg(current_time * buffer_size, ios::beg);

	// Case1: it's the last block to read the file:
	if (total_times - current_time == 1){

		int BUFFER = file_size - current_time * buffer_size;

		char* buffer = new char[BUFFER];

		// Read into buffer:
		fin.read(buffer, BUFFER);


		// tokenize and stem symbol: put term's document id and count into map...
		symbol_process(buffer, BUFFER, file_index);

		read_size += BUFFER;

		free(buffer);

	}



	// Case2: If it's not the last block to read
	else {
		char* buffer = new char[buffer_size];
		fin.read(buffer, buffer_size);

		symbol_process(buffer, buffer_size, file_index);

		read_size += buffer_size;

		free(buffer);

	}

}




// F4: Write inverted index file into index_directory:
void write_index_file(char* index_folder){

	DIR *idx_dir;
	idx_dir = opendir(index_folder);

	// Get the path of destination folder:
	string folder_path = index_folder;

	// WINDOWS version:
	//folder_path = folder_path + "\\";

	// Linux version:
	folder_path = folder_path + '/';


	// If index directory does not exists, create directory
	if (idx_dir == NULL){
		string c = "mkdir -p " + folder_path;
		const char *command = c.c_str();
		system(command);
	}



	// Set the full path of index file:
	string file_path = folder_path + "index_" + no_idx_file;


	// -------------- Write index------------------//
	// Open a write stream //
	ofstream fout(file_path, ios::out);


	// Write inverted index into external index file:
	for( map<string, map<int,int>>::const_iterator ptr=inverted_index.begin();ptr!=inverted_index.end(); ptr++) {
		fout << ptr->first << ':';

		for( map<int,int>::const_iterator eptr=ptr->second.begin();eptr!=ptr->second.end(); eptr++){
			fout << eptr->first << " " << eptr->second << ';';
		}

		fout << '\n';

	}

	fout.close();


	// Clear inverted index in memory:
	inverted_index.clear();

	read_size = 0;
	no_idx_file++;




}



// F5. Build inverted index:
void build_inverted_index(char* dest_folder, char* index_folder){

	// 1. get the files' names and open each file:
	DIR *dest_dir;
	dest_dir = opendir(dest_folder);

	// Get the path of destination folder:
	string folder_path = dest_folder;

	// WINDOWS version:
	//folder_path = folder_path + "\\";

	// Linux version:
	folder_path = folder_path + '/';



	struct dirent *ep;

	if (dest_dir != NULL){

		while ( (ep = readdir(dest_dir)) != NULL){

			if (strcmp(ep->d_name, ".") != 0 && strcmp(ep->d_name, "..") != 0) {

				// 1. Get the file name in the destination folder one by one:
				string file_name =  ep->d_name;

				// put the file name and index into map
				file_name_index[file_index] = file_name;


				// 2. Read the file using buffer:
				string file_path = folder_path + file_name;

				// Firstly get the size of file:
				int file_size = get_file_size(file_path);

				// get the times required to read the complete file:
				int times = file_size/buffer_size;

				if (file_size%buffer_size > 0){
					times ++;
				}
				//cout << times;

				ifstream fin(file_path, ios::in);

				for (int i = 0; i < times; i++){
					read_file(fin, i, times, file_size, file_index);

					// If the read file size exceeds the threshold, write into external file:
					if (read_size >= read_thresh){
						write_index_file(index_folder);
					}

				}

				fin.close();

				file_index ++;

			}


		}
		(void) closedir(dest_dir);

	}


	// Write inverted index into index_directory:
	write_index_file(index_folder);


}


// Function: process matched terms and their postings:
void matching_process (string term, string posting){

	//cout << term << ':' << posting << endl;

	map<string, map<int,int>>::iterator it = matched.find(term);

	// Case: If the term can be found in the matched map:
	if (it != matched.end()){
		string file_id = "";
		string count = "";

		bool record_file_id = true;

		// Go through the whole posting of the term:
		for (char ch : posting){

			// If current pointer is in file_id:
			if (record_file_id == true){
				if (ch == ' '){record_file_id = false;}
				else{file_id+=ch;}
			}

			// else if current pointer is in term count
			else {
				if (ch == ';'){
					record_file_id = true;

					int file = atoi(file_id.c_str());
					int count_value = atoi(count.c_str());

					//cout << file_id << ' ' <<count_value << endl;

					// --------------------------------- //
					map<int,int>::iterator it1 = it->second.find(file);
					if (it1 != it->second.end()){
						matched[term][file] += count_value;
					}
					else{
						matched[term][file] = count_value;
					}
					// --------------------------------- //

					file_id = "";
					count = "";

				}
				else{count+=ch;}
			}

		}

	}


	// Case: if it's first time to write matched posting for this term
	else{

		string file_id = "";
		string count = "";

		bool record_file_id = true;

		// Go through the whole posting of the term:
		for (char ch : posting){

			// If current pointer is in file_id:
			if (record_file_id == true){
				if (ch == ' '){record_file_id = false;}
				else{file_id+=ch;}
			}

			// else if current pointer is in term count
			else {
				if (ch == ';'){
					record_file_id = true;

					int file = atoi(file_id.c_str());
					int count_value = atoi(count.c_str());

					//cout << file_id << ' ' <<count_value << endl;
					matched[term][file] = count_value;

					file_id = "";
					count = "";

				}
				else{count+=ch;}
			}

		}

	}


}



// Function: keyword search in a single index:
void search_keyword_in_index(ifstream& fin, int buffer_size, vector<string> search_terms){

	// Read the whole index file into buffer:
	char* buffer = new char[buffer_size];
	fin.read(buffer, buffer_size);

	// Get the first term in search terms vector
	string search_term = search_terms.front();
	search_terms.erase(search_terms.begin());

	//
	string word = "";
	bool record_word = true;

	string posting = "";
	bool record_posting = false;

	// Go through all the index file
	for (int i=0; i<buffer_size; i++){
		char cha = buffer[i];

		// if current pointer is in term
		if (record_word == true){

			// If ':' is met, compare the recorded string with the search term:
			if (cha == ':'){
				record_word = false;

				if (word == search_term){
					record_posting = true;
				}
				else if (word < search_term){
					word = "";
				}

				else{
					if (search_terms.size() == 0){break;}

					search_term = search_terms.front();
					search_terms.erase(search_terms.begin());

					if (word == search_term){
						record_posting = true;
					}
					else{
						word ="";
					}
				}


			}
			else{word += cha;}
		}

		// if current pointer is in posting
		else{
			if (record_posting == true){

				// !!! TIME TO PUT THE MATCHED POSTING INTO MAP!!!! //
				if(cha == '\n'){
					record_posting = false;
					record_word = true;

					//
					matching_process(word, posting);
					//

					word = "";
					posting = "";


					if (search_terms.size() == 0){break;}
					search_term = search_terms.front();
					search_terms.erase(search_terms.begin());
				}

				else{posting += cha;}
			}

			else{
				if(cha == '\n'){record_word = true;}
			}
		}

	}

	free(buffer);

}


// Function: Keyword search
void keyword_search(vector<string> search_terms, char* index_folder){

	// Step 1: get all index file name under index folder:

	string folder_path = index_folder;
	folder_path = folder_path + '/';

	DIR *idx_dir;
	idx_dir = opendir(index_folder);
	struct dirent *ep;

	if (idx_dir != NULL){
		while ( (ep = readdir(idx_dir)) != NULL){

			if (strcmp(ep->d_name, ".") != 0 && strcmp(ep->d_name, "..") != 0) {
				string idx_name =  ep->d_name;

				// Get all index files other than filename_to_index mapping
				if (idx_name != "filename_index"){

					string idx_path = folder_path + idx_name;

					//cout << idx_name << endl;

					int file_size = get_file_size(idx_path);

					// Search through the index files:
					ifstream fin(idx_path, ios::in);

					search_keyword_in_index(fin, file_size, search_terms);

					fin.close();


				}

			}
		}
		(void) closedir(idx_dir);
	}


}



// Function: lexicographical compare 2 terms:
static bool compare_terms(string t1, string t2) {
	return t1 < t2;
}


// Function: sort the file_id based on the count of terms:
map<int,int> match_intersection(unsigned int term_num){

	// Initialize a result map <file_id: count>:
	map<int,int> result;
	map<int,int> tmp;

	if (matched.size() < term_num){
		return result;
	}

	else{

	// Go through the matched results:
	for( map<string, map<int,int>>::const_iterator ptr=matched.begin();ptr!=matched.end(); ptr++) {

		// If there is only 1 term_posting in map:
		if (result.empty()){
			result = ptr->second;
		}

		// If there are more than 1 term_postings in map: do intersection
		else{

			for( map<int,int>::const_iterator eptr=ptr->second.begin();eptr!=ptr->second.end(); eptr++){

				map<int,int>::iterator it = result.find(eptr->first);
				if (it != result.end()){
					int FID = eptr->first;
					int COUNT = eptr->second + it->second;

					tmp[FID] = COUNT;
				}

			}

			result = tmp;
			tmp.clear();

		}

	}
	}

	return result;

}



// Function: filename mapping
vector<pair<string,int>> filename_mapping(map<int,int> intersection, char* index_folder){

	vector<pair<string,int>> filename_count;

	// Open the filename_to_index map in the external file:
	string folder_path = index_folder;
	folder_path = folder_path + '/';

	string idx_path = folder_path + "filename_index";


	string line;

	int file_id = 0;
	string offset;
	string file_name;

	// Search through the index files:
	ifstream fin(idx_path, ios::in);

	for( map<int, int> ::const_iterator ptr=intersection.begin();ptr!=intersection.end(); ptr++) {
			//cout << ptr->first << ':'<< ptr->second << '\n';

			while(file_id != ptr->first){
				getline(fin,line);

				offset = line.substr(0, line.find("/"));
				file_id = atoi(offset.c_str());

				file_name = line.substr(line.find("/") + 1);

			}
			pair<string,int> pair (file_name, ptr->second);
			filename_count.push_back(pair);

	}

	fin.close();


	return filename_count;

}



// Function: compare pairs
static bool compare_pair(pair<string,int> p1, pair<string,int> p2) {

	if (p1.second == p2.second){
		return p1.first < p2.first;
	}
	return p1.second > p2.second;

}


// Function: Output Result
void output_result(vector<pair<string,int>> intersection){

	for (pair<string,int> pair : intersection){
		cout << pair.first << '\n';
		//cout << pair.first << pair.second << '\n';
	}

}






int main(int argc, char* argv[]) {

	// 1. destination folder //
	char* dest_folder = argv[1];

	// 2. Index folder to be built //
	char* index_folder = argv[2];


	// If Index folder exists, do not need to build index, else, BUILD INVERTED INDEX.
	DIR *idx_dir;
	idx_dir = opendir(index_folder);

	if (idx_dir != NULL){
		//cout<< "no need to build index!" << endl;
	}

	else{
		// Step 1 : Write inverted index into index directory:
		build_inverted_index(dest_folder, index_folder);


		// Step 2: Write filename_to_index map into index directory:
		string idx_folder = index_folder;

		string folder_path = idx_folder + '/' + "filename_index";
		ofstream fout(folder_path, ios::out);

		for( map<int, string> ::const_iterator ptr=file_name_index.begin();ptr!=file_name_index.end(); ptr++) {
			fout << ptr->first <<'/'<< ptr->second << '\n';
		}

		fout.close();
		file_name_index.clear();

	}


	// 3. Accept 1-5 search terms
	vector<string> search_terms;
	for (int x=3; x<argc; x++) {
		string search_term = string(argv[x]);

		// lower case and stem search term:
		transform(search_term.begin(), search_term.end(), search_term.begin(), ::tolower);
		Porter2Stemmer::stem(search_term);

		search_terms.push_back(search_term);
	}

	// lexicographically sort search terms:
	sort(search_terms.begin(), search_terms.end(), compare_terms);
	//for (string t: search_terms){cout << t << endl;}


	// 4. Perform keyword search (using built index folder): put matched results into a map:
	keyword_search(search_terms, index_folder);


	// 5. Intersection of matched result:
	map<int,int> intersection = match_intersection(argc-3);


	// 6. Output:
	if (intersection.size()==0){
		cout << '\n';
	}
	else{
		// Get the mapping of file_index to file_name:
		vector<pair<string,int>> filename_count = filename_mapping(intersection, index_folder);

		sort(filename_count.begin(), filename_count.end(), compare_pair);

		// Sort based on term count
		output_result(filename_count);
	}



	return 0;

}
