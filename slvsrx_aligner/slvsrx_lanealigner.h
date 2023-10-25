//#include "/opt/Xilinx/Vivado/2019.2/include/gmp.h"
#ifndef __SLVS_RX_LANE_ALIGN
#define __SLVS_RX_LANE_ALIGN
#include <hls_stream.h>
#include <ap_int.h>
#include <ap_axi_sdata.h>
//#include <hls_video.h"

struct axiWord {
 ap_uint<36> data;  // 31:0  -> data,  35:32 -> charbit  , 0 is data 1 is char
 //ap_uint<8> strb;
 ap_uint<1> last;
};

//typedef hls::stream<ap_axiu<128,1,1,1> > axiPixel;
typedef ap_axiu<128,1,1,1 >   axiPixel;
//struct axiPixel {
// ap_uint<128> data;  // 31:0  -> data,  35:32 -> charbit  , 0 is data 1 is char
// ap_uint<1> last;
//};


void slvsrx_lanealigner(hls::stream<axiWord> &inData, hls::stream<axiWord> &outData);

void slvsrx_merger(hls::stream<axiWord> &inData0,
		hls::stream<axiWord> &inData1,
		hls::stream<axiWord> &inData2,
		hls::stream<axiWord> &inData3,
		hls::stream<axiPixel> &outData,
		bool *framestart,
		bool *frameend,
		bool *linevalid,
		ap_uint<16> *curline,
		ap_uint<16> *lastPacketSize
		);

//void slvsrx_merger_test(hls::stream<axiWord> &inData0,
//		hls::stream<axiWord> &inData1,
//		hls::stream<axiWord> &inData2,
//		hls::stream<axiWord> &inData3,
//		hls::stream<axiPixel> &outData,
//		bool *framestart,
//		bool *frameend,
//		bool *linevalid,
//		ap_uint<16> *curline,
//		ap_uint<16> *lastPacketSize
//		);

void slvsrx_merger_test(hls::stream<axiWord> &inData0,
		hls::stream<axiWord> &inData1,
		hls::stream<axiWord> &inData2,
		hls::stream<axiWord> &inData3,
		hls::stream<axiPixel> &outData,
		bool *framestart,
		bool *frameend,
		bool *linevalid,
		ap_uint<16> *curline,
		ap_uint<16> *lastPacketSize
		) ;

#endif
