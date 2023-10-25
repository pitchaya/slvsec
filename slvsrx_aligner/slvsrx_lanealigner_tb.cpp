#include <hls_stream.h>
#include <ap_int.h>
#include "slvsrx_lanealigner.h"
#include <vector>

using namespace hls;
using namespace std;

int getColumnIdx(const char *line, const char * key) {
	int idx = 0;
	int len_key = strlen(key);
	int len_line = strlen(line);
	for(int i=0;i<len_line;i++) {
		if (i == 0 || line[i] == ',') {
			 if (line[i] == ',')  {
				 idx++;
				 i++;
			 }
//			 printf("%s\n", line + i);
			if (strncmp(line + i, key, len_key) == 0) {
				// Checking prefer
				if ((i + len_key == len_line) || (line[i + len_key] == ',') || (line[i + len_key] == '\n')) {
					return idx;
				}
			}
		}
	}
	return -1;
}

void getColumnWord( const char *line, char * sword, int idx) {
	int sidx = 0;
	int cidx = 0;
	int len_line = strlen(line);
//	printf("%d %d\n", len_line, idx);
	for(int i=0;i<len_line;i++) {
		if (line[i] == ',') {
			cidx ++;
		} else {
			if (cidx == idx) {
				sword[sidx++] = line[i];
			}
		}
		if (cidx > idx) break;
	}
	sword[sidx] = 0;
}

typedef struct {
		const char *key;
		int hbit;
		int lbit;
		int type;
	} key_map;

#if 0
key_map defaultmap0[] ={
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0_S_RX0_TDATA[35:35]",  35, 35, 0},
		{ "u_ila_0_gt_wrapper_0_S_RX0_TDATA[34:32]",  34, 32, 0},
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0/inst/gtwizard_0_support_i/gtwizard_0_init_i/inst/gtwizard_0_i/gt0_gtwizard_0_i/gt0_rxdata_out[31:0]",  31, 0, 1},
		{ NULL, 0, 0, 0}

};

key_map defaultmap1[] ={
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0_S_RX1_TDATA[35:35]",  35, 35, 0},
		{ "u_ila_0_gt_wrapper_0_S_RX1_TDATA[34:32]",  34, 32, 0},
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0/inst/gtwizard_0_support_i/gtwizard_0_init_i/inst/gtwizard_0_i/gt1_gtwizard_0_i/gt1_rxdata_out[31:0]",  31, 0, 1},
		{ NULL, 0, 0, 0}

};

key_map defaultmap2[] ={
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0_S_RX2_TDATA[35:35]",  35, 35, 0},
		{ "u_ila_0_gt_wrapper_0_S_RX2_TDATA[34:32]",  34, 32, 1},
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0/inst/gtwizard_0_support_i/gtwizard_0_init_i/inst/gtwizard_0_i/gt2_gtwizard_0_i/gt2_rxdata_out[31:0]",  31, 0, 1},
		{ NULL, 0, 0, 0}

};

key_map defaultmap3[] ={
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0_S_RX3_TDATA[35:35]",  35, 35, 0},
		{ "u_ila_0_gt_wrapper_0_S_RX3_TDATA[34:32]",  34, 32, 0},
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0/inst/gtwizard_0_support_i/gtwizard_0_init_i/inst/gtwizard_0_i/gt3_gtwizard_0_i/gt3_rxdata_out[31:0]",  31, 0, 1},
		{ NULL, 0, 0, 0}

};
#endif
#if 0
key_map defaultmap0[] ={
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0_S_RX0_TDATA[35:32]",  35, 32, 1},
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0/inst/gtwizard_0_support_i/gtwizard_0_init_i/inst/gtwizard_0_i/gt0_gtwizard_0_i/gt0_rxdata_out[31:0]",  31, 0, 1},
		{ NULL, 0, 0, 0}

};

key_map defaultmap1[] ={
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0_S_RX1_TDATA[35:32]",  35, 32, 1},
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0/inst/gtwizard_0_support_i/gtwizard_0_init_i/inst/gtwizard_0_i/gt1_gtwizard_0_i/gt1_rxdata_out[31:0]",  31, 0, 1},
		{ NULL, 0, 0, 0}

};

key_map defaultmap2[] ={
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0_S_RX2_TDATA[35:32]",  35, 32, 1},
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0/inst/gtwizard_0_support_i/gtwizard_0_init_i/inst/gtwizard_0_i/gt2_gtwizard_0_i/gt2_rxdata_out[31:0]",  31, 0, 1},
		{ NULL, 0, 0, 0}

};

key_map defaultmap3[] ={
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0_S_RX3_TDATA[35:32]",  35, 32, 1},
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0/inst/gtwizard_0_support_i/gtwizard_0_init_i/inst/gtwizard_0_i/gt3_gtwizard_0_i/gt3_rxdata_out[31:0]",  31, 0, 1},
		{ NULL, 0, 0, 0}

};

#endif
#if 0
key_map defaultmap0[] ={
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0_S_RX0_TDATA[35:32]",  35, 32, 1},
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0/inst/gtwizard_0_support_i/gtwizard_0_init_i/inst/gtwizard_0_i/gt0_gtwizard_0_i/gt0_rxdata_out[31:0]",  31, 0, 1},
		{ NULL, 0, 0, 0}

};

key_map defaultmap1[] ={
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0_S_RX1_TDATA[35:32]",  35, 32, 1},
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0/inst/gtwizard_0_support_i/gtwizard_0_init_i/inst/gtwizard_0_i/gt1_gtwizard_0_i/gt1_rxdata_out[31:0]",  31, 0, 1},
		{ NULL, 0, 0, 0}

};

key_map defaultmap2[] ={
		{ "u_ila_0_gt_wrapper_0_S_RX2_TDATA[35:32]",  35, 32, 1},
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0/inst/gtwizard_0_support_i/gtwizard_0_init_i/inst/gtwizard_0_i/gt2_gtwizard_0_i/gt2_rxdata_out[31:0]",  31, 0, 1},
		{ NULL, 0, 0, 0}

};

key_map defaultmap3[] ={
		{ "u_ila_0_gt_wrapper_0_S_RX3_TDATA[35:32]",  35, 32, 1},
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0/inst/gtwizard_0_support_i/gtwizard_0_init_i/inst/gtwizard_0_i/gt3_gtwizard_0_i/gt3_rxdata_out[31:0]",  31, 0, 1},
		{ NULL, 0, 0, 0}

};
#endif

#if 0
key_map defaultmap0[] ={
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0_S_RX0_TDATA[35:32]",  35, 32, 1},
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0/inst/gtwizard_0_support_i/gtwizard_0_init_i/inst/gtwizard_0_i/gt0_gtwizard_0_i/gt0_rxdata_out[31:0]",  31, 0, 1},
		{ NULL, 0, 0, 0}

};

key_map defaultmap1[] ={
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0_S_RX1_TDATA[35:32]",  35, 32, 1},
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0/inst/gtwizard_0_support_i/gtwizard_0_init_i/inst/gtwizard_0_i/gt1_gtwizard_0_i/gt1_rxdata_out[31:0]",  31, 0, 1},
		{ NULL, 0, 0, 0}

};

key_map defaultmap2[] ={
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0_S_RX2_TDATA[35:32]",  35, 32, 1},
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0/inst/gtwizard_0_support_i/gtwizard_0_init_i/inst/gtwizard_0_i/gt2_gtwizard_0_i/gt2_rxdata_out[31:0]",  31, 0, 1},
		{ NULL, 0, 0, 0}

};

key_map defaultmap3[] ={
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0_S_RX3_TDATA[35:32]",  35, 32, 1},
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0/inst/gtwizard_0_support_i/gtwizard_0_init_i/inst/gtwizard_0_i/gt3_gtwizard_0_i/gt3_rxdata_out[31:0]",  31, 0, 1},
		{ NULL, 0, 0, 0}

};
#endif


#if 1
key_map defaultmap0[] ={
//		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0_S_RX0_TDATA[35:32]",  35, 32, 1},
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0_S_RX0_TDATA[35:0]",  35, 0, 1},
//		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0/inst/gtwizard_0_support_i/gtwizard_0_init_i/inst/gtwizard_0_i/gt0_gtwizard_0_i/gt0_rxdata_out[31:0]",  31, 0, 1},
		{ NULL, 0, 0, 0}

};

key_map defaultmap1[] ={
//		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0_S_RX1_TDATA[35:32]",  35, 32, 1},
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0_S_RX1_TDATA[35:0]",  35, 0, 1},
//		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0/inst/gtwizard_0_support_i/gtwizard_0_init_i/inst/gtwizard_0_i/gt1_gtwizard_0_i/gt1_rxdata_out[31:0]",  31, 0, 1},
		{ NULL, 0, 0, 0}

};

key_map defaultmap2[] ={
//		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0_S_RX2_TDATA[35:32]",  35, 32, 1},
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0_S_RX2_TDATA[35:0]",  35, 0, 1},
//		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0/inst/gtwizard_0_support_i/gtwizard_0_init_i/inst/gtwizard_0_i/gt2_gtwizard_0_i/gt2_rxdata_out[31:0]",  31, 0, 1},
		{ NULL, 0, 0, 0}

};

key_map defaultmap3[] ={
		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0_S_RX3_TDATA[35:0]",  35, 0, 1},
//		{ "test_design_i/slvsecblock_1/inst/gt_wrapper_0/inst/gtwizard_0_support_i/gtwizard_0_init_i/inst/gtwizard_0_i/gt3_gtwizard_0_i/gt3_rxdata_out[31:0]",  31, 0, 1},
		{ NULL, 0, 0, 0}

};
#endif

void extract( const std::vector< ap_uint<36> > &words, std::vector< ap_uint<9> > &ebytes) {
	for(int i=0;i< words.size();i++) {

		for(int k=0;k<4;k++) {
			ap_uint<9> b = 0;
			b.range(7,0) = words[i].range(k*8 +7, k*8);
			b.set_bit(8 ,  words[i].get_bit( k+ 32));
			ebytes.push_back(b);
		}
	}

}

char line[65535];

void read_waveform_csv(const char *filename, std::vector< ap_uint<36> > & words, key_map *maps) {
	std::vector< int > colIdx ;
	char sword[256];
	//{ "u_ila_0_gt_wrapper_0_S_RX0_TDATA[34:32]" , 34, 32 }
	FILE *fi = fopen(filename, "rb");
	int sIdx = 0;

	fgets(line, 65535, fi);
	while( maps[sIdx].key != NULL) {
		int idx =  getColumnIdx(line, maps[sIdx].key);
		colIdx.push_back(idx);
		printf("%s -> %d\n", maps[sIdx].key, idx);
		printf("%d %d %d\n", maps[sIdx].hbit, maps[sIdx].lbit, maps[sIdx].type);
		if (idx == -1) {
			printf("failed\n");
			return;
		}
		sIdx++;
	}

	// skip
	fgets(line, 65535, fi);
	while(	fgets(line, 65535, fi)!= NULL) {
		ap_uint<36> word = 0;
		for(int i=0;i<sIdx;i++) {
			//void getColumnWord( const char *line, char * sword, int idx) {
			getColumnWord( line, sword, colIdx[i]);
//			printf("%s - ", sword);
			// Binary
			if (maps[i].type == 0) {
				word.range(maps[i].hbit, maps[i].lbit) = ap_uint< 40>(sword ,2 );
//				int slen = strlen(sword);
//				for(int k=0;k<slen;k++) {
//					if (sword[k] == '1')
//						word.set_bit( maps[i].lbit + (slen - k -1) , 1);
//
//				}
			} else {
				word.range(maps[i].hbit, maps[i].lbit) = ap_uint< 40>(sword ,16 );
//				std::cout << std::hex << ap_uint< 32>(sword ,16 );
//				int val;
//				sscanf(sword, "%x", &val);
//				word.range(maps[i].hbit, maps[i].lbit) = val;
//				printf("%x\n", val);
			}
		}
//		std::cout << std::hex << word << std::endl;
//		std::cout << "OUT:" << std::hex << std::setw(16) << word  << std::endl;
		words.push_back(word);

//		printf("\n");
	}

	fclose(fi);

}

const ap_uint<8>  PADCHAR0("f7", 16);
const ap_uint<8>  PADCHAR1("9c", 16);
const ap_uint<8>  PADCHAR2("dc", 16);
const ap_uint<8>  PADCHAR3("7c", 16);
const ap_uint<36> IDLE_CODE("000000000",16);
const ap_uint<36> SYNC_CODE("fbcbcbcbc",16);  // this is K.28.5
const ap_uint<36> START_CODE("ffb5cfbbc",16); // This is K.28.5 K27.7 K.28.2, K27.7
const ap_uint<36>  STOP_CODE("ffdfefdbc",16); // This is K.28.5 K29.7 K.30.7, K29.7

ap_uint<36> assemble( ap_uint<9> w0, ap_uint<9> w1, ap_uint<9> w2, ap_uint<9> w3) {
	ap_uint<36> ret = 0;
	ret.range(7,0) = w0.range(7,0);
	ret.range(7+8,8) = w1.range(7,0);
	ret.range(7+16,16) = w2.range(7,0);
	ret.range(7+24,24) = w3.range(7,0);
	ret.set_bit( 32, w0.get_bit(8));
	ret.set_bit( 33, w1.get_bit(8));
	ret.set_bit( 34, w2.get_bit(8));
	ret.set_bit( 35, w3.get_bit(8));

	return ret;
}

static bool isPad( ap_uint<8> inp) {
	return (inp == PADCHAR0 || inp == PADCHAR1 || inp==PADCHAR2 || inp == PADCHAR3);
}

void remove_pad( std::vector< ap_uint<9> > &ewords, std::vector< ap_uint<9> > &ewords_rem) {
	for(int i=0;i<ewords.size();i++) {
		if (!(ewords[i].get_bit(8) && isPad(ewords[i].range(7,0)))) {
			ewords_rem.push_back(ewords[i]);
		}

	}
}

void filter_active(std::vector< ap_uint<9> > &ewords_rem, std::vector< ap_uint<9> > &active_words ) {
	int active = 0;
		for(int i=0;i<ewords_rem.size()-4;i++) {
			ap_uint<36> ew = assemble(ewords_rem[ i+0], ewords_rem[i+1], ewords_rem[i+2], ewords_rem[i+3]);
			if (ew == START_CODE) {
				active = 1;
			}
			if (ew == STOP_CODE) {
				active = 0;
				active_words.push_back(ewords_rem[i + 0]);
				active_words.push_back(ewords_rem[i + 1]);
				active_words.push_back(ewords_rem[i + 2]);
				active_words.push_back(ewords_rem[i + 3]);

			}
			if (active)
				active_words.push_back(ewords_rem[i]);
//				std::cout << "OUT:" << std::hex << std::setw(16) <<  ew << "  " <<ewords_rem[i] << std::endl;
		}
}

void filter_active4(std::vector< ap_uint<9> > &ewords_rem, std::vector< ap_uint<36> > &active_words ) {
	int active = 0;
	int active_k = 0;
		for(int i=0;i<ewords_rem.size()-4;i++) {
			ap_uint<36> ew = assemble(ewords_rem[ i+0], ewords_rem[i+1], ewords_rem[i+2], ewords_rem[i+3]);
			if (ew == START_CODE) {
				active = 1;
				active_k = 0;
			}
			if (ew == STOP_CODE) {
				active = 0;
				active_words.push_back(ew);
//				active_words.push_back(ewords_rem[i + 1]);
//				active_words.push_back(ewords_rem[i + 2]);
//				active_words.push_back(ewords_rem[i + 3]);

			}
			if (active) {
				if (active_k % 4 == 0)
					active_words.push_back(ew);
			}
			active_k++;
//				std::cout << "OUT:" << std::hex << std::setw(16) <<  ew << "  " <<ewords_rem[i] << std::endl;
		}
}

std::vector< ap_uint<9> > readFilter(const char *filename,  key_map *maps) {
	std::vector< ap_uint<36> > words;
	std::vector< ap_uint<9> > ewords;
	std::vector< ap_uint<9> > ewords_rem;
	std::vector< ap_uint<9> > ewords_rem_f;
	read_waveform_csv( filename, words, maps);
	extract(words, ewords);
	remove_pad(ewords, ewords_rem);
//	ewords_rem = ewords;
	filter_active( ewords_rem, ewords_rem_f);
	return  ewords_rem_f;


}


std::vector< ap_uint<36> > readFilter4(const char *filename,  key_map *maps) {
	std::vector< ap_uint<36> > words;
	std::vector< ap_uint<9> > ewords;
	std::vector< ap_uint<9> > ewords_rem;
	std::vector< ap_uint<36> > ewords_rem_f;
	read_waveform_csv( filename, words, maps);
	extract(words, ewords);
	printf("EX: %d %d\n", words.size(), ewords.size());
	remove_pad(ewords, ewords_rem);
//	ewords_rem = ewords;
	filter_active4( ewords_rem, ewords_rem_f);
	return  ewords_rem_f;


}

int main() {
#if 0
	std::vector< ap_uint<36> > words;
	std::vector< ap_uint<9> > ewords;
	std::vector< ap_uint<9> > ewords_rem;
	std::vector< ap_uint<9> > ewords_rem_f;
	read_waveform_csv( "/home/catus/fpga/waveform.csv", words, defaultmap2);
	extract(words, ewords);
	remove_pad(ewords, ewords_rem);
	filter_active( ewords_rem, ewords_rem_f);
//	ewords_rem_f = ewords_rem;


	//int

	hls::stream<axiWord> inData;
	hls::stream<axiWord> outData;
	std::vector< ap_uint<36> > modulewords;
	std::vector< ap_uint<9> > e_modulewords;
	for(int i=0;i<words.size();i++) {
		axiWord datin;
		datin.data = words[i];
		datin.last = 0;
		axiWord datout;
		inData.write(datin);
		slvsrx_lanealigner(inData, outData);

		if (!outData.empty()) {
			outData.read(datout);
			modulewords.push_back(datout.data);
//			std::cout << std::hex << std::setw(36) << words[i]  <<  " : -> " << datout.data << std::endl;
//			std::cout << std::hex << std::setw(36) << datout.data << std::endl;
		}
	}
	extract(modulewords, e_modulewords);
#endif


	std::vector< ap_uint<36> > wordsArray[4];
	hls::stream<axiWord> inDataArray[4];

	key_map * maps[] = { defaultmap0, defaultmap1, defaultmap2, defaultmap3};

	wordsArray[0].push_back(0);
	for(int k=0;k<4;k++)
	{
//		std::vector< ap_uint<36> > words;

//		read_waveform_csv( "/home/catus/fpga/wave4096_0.csv", wordsArray[k], maps[k]);
//		read_waveform_csv( "/home/catus/fpga/wave4096_1.csv", wordsArray[k], maps[k]);
//		read_waveform_csv( "/home/catus/fpga/wave4096_2.csv", wordsArray[k], maps[k]);
//		read_waveform_csv( "/home/catus/fpga/wave4096_3.csv", wordsArray[k], maps[k]);
//		read_waveform_csv( "/home/catus/fpga/iladata9.csv", wordsArray[k], maps[k]);

		//read_waveform_csv( "/home/catus/fpga/cam1_400mv/waveform.csv", wordsArray[k], maps[k]);
		read_waveform_csv( "/home/pitchaya/borom/fpga/cambuf/waveform.csv", wordsArray[k], maps[k]);

	}

//	read_waveform_csv( "/home/catus/fpga/wave4096_0.csv", wordsArray[1], defaultmap1);
//	read_waveform_csv( "/home/catus/fpga/wave4096_0.csv", wordsArray[2], defaultmap2);
//	read_waveform_csv( "/home/catus/fpga/wave4096_0.csv", wordsArray[3], defaultmap3);

	//write_hw_ila_data -csv_file {/home/catus/fpga/wave4096_1.csv} hw_ila_data_1

	bool framestart, frameend , linevalid;
	ap_uint<16> curline;
	ap_uint<16> lastSizeCnt;
//	struct axiPixel {
//	 ap_uint<128> data;  // 31:0  -> data,  35:32 -> charbit  , 0 is data 1 is char
//	 ap_uint<1> last;
//	};

	hls::stream<axiPixel> outPixel;
	//wordsArray[0].insert(__position, __first, __last)
	// Let's offset word by 12 bytes
//	for(int i=0;i<wordsArray[0].size()-12;i++)
//		wordsArray[0][i] = wordsArray[0][i+12];
	int countBeat = 0;
	for(int i=0;i<wordsArray[0].size();i++) {
		for(int k=0;k<4;k++) {
			axiWord datin;
			datin.data = wordsArray[k][i];
			datin.last = 0;
			axiWord datout;
			inDataArray[k].write(datin);
		}
//		std::cout << std::dec << i << "   " << i % 4 << " " <<  std::hex << wordsArray[0][i] << std::endl;

		slvsrx_merger_test( inDataArray[0], inDataArray[1], inDataArray[2], inDataArray[3],   outPixel,
				&framestart, &frameend , &linevalid, &curline, &lastSizeCnt);
//		std::cout << curline <<  "   " << lastSizeCnt << "\n";
		if (!outPixel.empty()) {
			axiWord idat[4];
			for(int k=0;k<4;k++)
				idat[k].data = 0;
			axiPixel pixOut = outPixel.read();
			( idat[0].data.range(31,24),  idat[1].data.range(31,24), idat[2].data.range(31,24), idat[3].data.range(31,24),
			  idat[0].data.range(23,16),  idat[1].data.range(23,16), idat[2].data.range(23,16), idat[3].data.range(23,16),
			  idat[0].data.range(15, 8),  idat[1].data.range(15, 8), idat[2].data.range(15, 8), idat[3].data.range(15, 8),
			  idat[0].data.range( 7, 0),  idat[1].data.range( 7, 0), idat[2].data.range( 7, 0), idat[3].data.range( 7, 0)) = pixOut.data;
//			std::cout << std::dec << i << "   " << i % 4 << " " <<  std::hex << pixOut.data << "  FS  " << framestart << "  FE  " << frameend << "  LV  " << linevalid << "  CL  " << curline <<  " user: " << pixOut.user << "\n";

			if (1)
			std::cout << std::dec << i << "   " << i % 4 << " " <<
					std::hex <<	idat[0].data << " " <<
					std::hex <<	idat[1].data << " " <<
					std::hex <<	idat[2].data << " " <<
					std::hex <<	idat[3].data << " " <<
				"  FS  " << framestart << "  FE  " << frameend << "  LV  " << linevalid << "  CL  " << curline <<  " user: " << pixOut.user << "  " << lastSizeCnt << "\n";
			countBeat++;
			if (pixOut.last) {

				std::cout << "######################## - " << std::dec<< countBeat << std::endl;
				countBeat = 0;
			}
		}
	}

	int mytest = 1;
	if (mytest) {
//	for(int i=0;i<ewords_rem.size() ;i++) {
//		std::cout << std::hex << std::setw(12) <<  ewords_rem[i] << std::endl;
//	}
		std::vector< ap_uint<36> >  wavefs[4];
		for(int k=0;k<4;k++)
			wavefs[k] = readFilter4( "/home/catus/fpga/cambuf/waveform.csv", maps[k]);
		countBeat = 0;
		printf("Size: %d %d %d %d\n", wavefs[0].size(), wavefs[1].size(), wavefs[2].size(), wavefs[3].size());
		for(int i=0;i<wavefs[0].size() ;i++) {
			std::cout << std::dec << i % 4 << " " <<  std::hex << std::setw(12) <<  (wavefs[0][i]) << " " << (wavefs[1][i]) <<  " "<< (wavefs[2][i])<<  " "<< (wavefs[3][i]) << "\n" ; //"     ###   " << e_modulewords[i] <<  std::endl;

			countBeat++;
			if (wavefs[0][i] == STOP_CODE) {
				std::cout << "######################## - " << std::dec<< countBeat << std::endl;
				countBeat = 0;
	//			0xffb5cfbbc
			}
		}
	}

/*
	hls::stream<ap_uint<9> > inOffsetData;
	#pragma HLS STREAM variable = inOffsetData depth = 16;

	hls::stream<axiWord> inData;
	hls::stream<axiWord> outData;
	axiWord datin;
	axiWord datout;

	const ap_uint<36> SYNC_CODE("fbcbcbcbc",16);  // this is K.28.5
	const ap_uint<36> START_CODE("ffb5cfbbc",16); // This is K.28.5 K27.7 K.28.2, K27.7
	const ap_uint<36>  STOP_CODE("ffdfefdbc",16); // This is K.28.5 K29.7 K.30.7, K29.7

	// Simple case first
	if(1)
	for(int i=0;i<100;i++) {

		datin.data = i;
		datout.last = 0;
//		if (i >= 10  && i < 90)
		if (i == 10)
			datin.data = START_CODE;
		if (i == 90) {
			datin.data = STOP_CODE;
		}
		inData.write(datin);
		slvsrx_lanealigner(inData, outData);
		if (!outData.empty()) {
			outData.read(datout);
			std::cout << std::hex << std::setw(36) << datout.data << std::endl;
		}
	}
	if (0)
	for(int datEnd = 79;datEnd <= 84;datEnd++) {
		for(int startAt =16; startAt < 21;startAt++) {
			int inStreamIdx = 0;
			int startSending = 0;
			int datIdx = 0;
			std::cout << std::dec<< "Starting At: " << startAt << " ending: " << datEnd << std::endl;
			for(int i=0;i<100;i++) {


				for(int k=0;k<4;k++) {
					if (startSending == 0){
						inOffsetData.write( ap_uint<9>("1bc", 16));

					} else {
						if (startSending <=4) {
							inOffsetData.write( ap_uint<9>(0x100 | ( START_CODE.range(startSending*8-1,(startSending-1)*8 ))));
							startSending++;
						} else {
							if (datIdx > datEnd) {
								inOffsetData.write( ap_uint<9>(0x1bc));
							} else {
								inOffsetData.write( ap_uint<9>(datIdx & 0xFF));
							}
								datIdx++;
						}
					}
					if (inStreamIdx == startAt)
						startSending = 1;
					inStreamIdx++;
				}
				//inOffsetData.write( ap_uint<9>("1bc", 9));
				//inOffsetData.write( ap_uint<9>("1bc", 9));
				//inOffsetData.write( ap_uint<9>("1bc", 9));

				ap_uint<40> sendingCode;

				for(int k=0;k<4;k++) {
					ap_uint<9> inB = inOffsetData.read();
		//			std::cout << "INB:" << std::hex << std::setw(8) << inB << std::endl;
					sendingCode.bit(k+32) = inB.bit(8);
					sendingCode.range((k+1)*8-1, k*8) = inB.range(7,0);
				}

				datin.data = sendingCode;
				datin.last = 0;

		//		std::cout << "IN :" << std::hex << std::setw(36) << datin.data << std::endl;
		//		if (i >= 10  && i < 90)
				inData.write(datin);
				slvsrx_lanealigner(inData, outData);
				if (!outData.empty()) {
					outData.read(datout);
					std::cout << "OUT:" << std::hex << std::setw(16) << datout.data  << " " << std::setw(2)  << datout.last << std::endl;
				}
			}
			while(!inOffsetData.empty())  inOffsetData.read();
		}
	}
*/
	return 0;
}
