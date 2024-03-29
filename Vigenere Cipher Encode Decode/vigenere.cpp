#include "vigenere.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <cctype>

using namespace std;

// Fill string vector with values, syntax similar to 2D vector
void genMatrix(string s, vector<string>& matrix) {
	for (int i = 0; i < s.size(); i++) {
		matrix.push_back(s);
		// Take the first character s and add it to the end of the strong
		s = s.substr(1, s.size() - 1) + s[0];
	}
}

// Map characters to indeces to be used when getting values from matrix
void setupIndexMap(unordered_map<char, int>& map, string s) {
	for (int i = 0; i < s.size(); i++)
		map.insert({ s[i], i });
}

// Given value in a matrix and its row, find the column
int findColumn(const vector<string>& matrix, int row, char key) {
	for (int i = 0; i < matrix.size(); i++)
		if (matrix[i][row] == key)
			return i;

	return -1;
}

// Default constructor
Vigenere::Vigenere() {
	genMatrix(m_alphabet, m_alphaMatrix);
	genMatrix(m_nonAlphaSymbols, m_nonAlphaMatrix);
	setupIndexMap(m_alphaIndeces, m_alphabet);
	setupIndexMap(m_nonAlphaIndeces, m_nonAlphaSymbols);
}

/* Debug, no longer needed

// Debug function to print current alphabet matrix
void Vigenere::printAlphaMatrix() {
	printMatrix(m_alphaMatrix);
}

// Debug function to print current non-alphabet matrix
void Vigenere::printNonAlphaMatrix() {
	printMatrix(m_nonAlphaMatrix);
}

// Prints values of 2D vector with newlines seperating rows and characters 1-P representing indeces.
void Vigenere::printMatrix(const vector<string>& matrix) {
	cout << "  ";

	int i;

	for (i = 0; i < matrix.size() && i < 10; i++)
		cout << i << " ";

	if (i > 9)
		for (i = 0; i + 10 < matrix.size(); i++)
			cout << char('A' + i) << " ";

	cout << "\n";
	for (i = 0; i < matrix.size() && i < 10; i++) {
		cout << i << " ";
		for (int j = 0; j < matrix[i + 10].size(); j++)
			cout << matrix[i][j] << " ";
		cout << "\n";
	}

	if (i > 9)
		for (i = 0; i + 10 < matrix.size(); i++) {
			cout << char('A' + i) << " ";
			for (int j = 0; j < matrix[i + 10].size(); j++)
				cout << matrix[i + 10][j] << " ";
			cout << "\n";
		}
}
*/

// Seperates string into its alphabetic chars and legal non-alphabetic chars
void Vigenere::extractAlphaNonAlpha(string& keyAlpha, string& keyNonAlpha, string keyPhrase) {
	for (int i = 0; i < keyPhrase.size(); i++) {
		if (isalpha(keyPhrase[i]))
			keyAlpha += keyPhrase[i];
		else if (m_nonAlphaIndeces.find(keyPhrase[i]) != m_nonAlphaIndeces.end())
			keyNonAlpha += keyPhrase[i];
	}
}

// Encrypt string with given key
string Vigenere::encrypt(string plaintext, string key) {
	string encrypted;

	string keyAlpha, keyNonAlpha;

	extractAlphaNonAlpha(keyAlpha, keyNonAlpha, key);

	// If no alphabetic characters are in key, do not encrypt alphabetic characters in plaintext
	if (keyAlpha == "") {
		keyAlpha = m_alphaMatrix[0][0];
	}

	// If no non-alphabetic characters are in key, do not encrypt non-alphabetic characters in plaintext
	if (keyNonAlpha == "") {
		keyNonAlpha = m_nonAlphaMatrix[0][0];
	}

	unordered_map<char, int>::iterator it;
	for (int i = 0, totalAlpha = 0, totalNonAlpha = 0, col, row; i < plaintext.size(); i++) {
		if (isalpha(plaintext[i])) {
			// Find index of current character from plaintext
			it = m_alphaIndeces.find(toupper(plaintext[i]));
			col = it->second;
			// Find index of current character from the key
			it = m_alphaIndeces.find(toupper(keyAlpha[totalAlpha % keyAlpha.size()]));
			row = it->second;

			// Use two indexes to find the cipher character by inputting into alpha matrix
			if (islower(plaintext[i]))
				encrypted += tolower(m_alphaMatrix[col][row]);
			else encrypted += m_alphaMatrix[col][row];
			totalAlpha++;
		}
		else if (m_nonAlphaIndeces.find(plaintext[i]) != m_nonAlphaIndeces.end()) {
			// Similar process to above
			it = m_nonAlphaIndeces.find(plaintext[i]);
			col = it->second;
			it = m_nonAlphaIndeces.find(keyNonAlpha[totalNonAlpha % keyNonAlpha.size()]);
			row = it->second;
			encrypted += m_nonAlphaMatrix[col][row];
			totalNonAlpha++;
		}
		// If the char isn't Az-Zz or a legal non-letter symbol, don't change it
		else encrypted += plaintext[i];
	}

	return encrypted;
}

// Decrypt string with given key
string Vigenere::decrypt(string ciphertext, string key) {
	string decrypted;

	string keyAlpha, keyNonAlpha;

	extractAlphaNonAlpha(keyAlpha, keyNonAlpha, key);

	// If no alphabetic characters are in key, do not decrypt alphabetic characters in ciphertext
	if (keyAlpha == "") {
		keyAlpha = m_alphaMatrix[0][0];
	}

	// If no non-alphabetic characters are in key, do not decrypt non-alphabetic characters in ciphertext
	if (keyNonAlpha == "") {
		keyNonAlpha = m_nonAlphaMatrix[0][0];
	}

	unordered_map<char, int>::iterator it;
	for (int i = 0, totalAlpha = 0, totalNonAlpha = 0, col, row; i < ciphertext.size(); i++) {
		if (isalpha(ciphertext[i])) {
			// Get the row using the key
			it = m_alphaIndeces.find(toupper(keyAlpha[totalAlpha % keyAlpha.size()]));
			row = it->second;
			// Find the column using the key and the current ciphertext char
			col = findColumn(m_alphaMatrix, row, toupper(ciphertext[i]));
			// We use the 0th char vector because it is guarenteed to be in order
			if (islower(ciphertext[i]))
				decrypted += tolower(m_alphaMatrix[0][col]);
			else decrypted += m_alphaMatrix[0][col];
			totalAlpha++;
		}
		// See above comments
		else if (m_nonAlphaIndeces.find(ciphertext[i]) != m_nonAlphaIndeces.end()) {
			it = m_nonAlphaIndeces.find(keyNonAlpha[totalNonAlpha % keyNonAlpha.size()]);
			row = it->second;
			col = findColumn(m_nonAlphaMatrix, row, ciphertext[i]);
			decrypted += m_nonAlphaMatrix[0][col];
			totalNonAlpha++;
		}
		// If the character is in neither matrix, place it without change
		else decrypted += ciphertext[i];
	}

	return decrypted;
}
