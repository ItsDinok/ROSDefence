#include "integrateAscon.h"
#include "test.txt"
using namespace std;

integrateAscon::integrateAscon(string path) {
    // No point repeating
    if (isFilesAscon) {
        return;
    }

    this->Path = path;
    string sourcePath = "test.txt";
    // Create input and output file streams
    ifstream source_file(sourcePath, ios::binary);
    ofstream dest_file(path, ios::binary);

    // Check if both file streams were opened successfully
    if (!source_file.is_open() || !dest_file.is_open()) {
        cerr << "Error: Failed to open file" << endl;
    }

    // Replace the contents of the destination file with the contents of the source file
    dest_file << source_file.rdbuf();

    // Close the file streams
    source_file.close();
    dest_file.close();

    // Print a success message
    cout << "File replaced successfully" << endl;
}

bool integrateAscon::isFilesAscon() {

}