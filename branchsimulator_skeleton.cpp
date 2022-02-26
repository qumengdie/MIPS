#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>

using namespace std;

int main (int argc, char** argv) {
	ifstream config;
	config.open(argv[1]);

	int m;
	config >> m;
 
	config.close();

	ofstream out;
	string out_file_name = string(argv[2]) + ".out";
	out.open(out_file_name.c_str());
	
	ifstream trace;
	trace.open(argv[2]);

    vector<int> counter(pow(2, m), 3);
	while (!trace.eof()) {
		unsigned long pc; bool taken;
		trace >> std::hex >> pc >> taken;
        
        unsigned long lsb;
        lsb = pc & static_cast<unsigned long>(pow(2, m)-1);
        
        bool prediction;
        if (counter[lsb] >> 1) prediction = true;
        else prediction = false;
        out << prediction << endl;
        
        if (taken){
            if (counter[lsb]<3) counter[lsb] = counter[lsb] + 1;
        } else {
            if (counter[lsb]>0) counter[lsb] = counter[lsb] - 1;
        }

	}
	 
	trace.close();	
	out.close();
}
