#include <hls_stream.h>
#include <ap_int.h>
#include <stdint.h>
#include <ap_axi_sdata.h>
//#include <hls_video.h"
#include <ap_fixed.h>
//typedef ap_fixed<16, 8, AP_RND >   Q;
//typedef ap_int<16>   Q;
typedef int16_t Q;


typedef ap_axiu<48,1,1,1 >   axiPixelIn;
typedef ap_axiu<24,1,1,1 >   axiPixelOut;

typedef ap_uint<8>   PixOut;
inline PixOut clamp(Q v, Q min, Q max) {
#pragma HLS PIPELINE II=1 enable_flush
	v >>= 6;
	if (v < min) v = 0;
	if (v > max) v =max;
	return v;
}

Q colormatrix[12] = { 1, 0, 0, 0,
					  0, 1, 0, 0,
					  0, 0, 1, 0};
inline ap_uint<24> color_mul(ap_uint<24> pixIn, Q colormatrix[12]) {
	Q B0 = pixIn.range(7,0);
	Q G0 = pixIn.range(15,8);
	Q R0 = pixIn.range(23,16);

	auto R0o = clamp(colormatrix[0] * R0 + colormatrix[1] * G0   + colormatrix[2] * B0 + colormatrix[3], 0, 255);
	auto G0o = clamp(colormatrix[4] * R0 + colormatrix[5] * G0   + colormatrix[6] * B0 + colormatrix[7], 0, 255);
	auto B0o = clamp(colormatrix[8] * R0 + colormatrix[9] * G0   + colormatrix[10] * B0 + colormatrix[11], 0, 255);
	ap_uint<24> outv = (R0o, G0o, B0o);
	return outv;
}



void vidConv( hls::stream< axiPixelIn> &vid_in, hls::stream<axiPixelOut> &vid_out,
		Q colormatrix[12], int *copy_shadow) {

//#pragma HLS PIPELINE II=4 enable_flush
#pragma HLS PIPELINE II=2 enable_flush
//	static int padPos = 0;
//	static axiWord padRemoveData;
#pragma HLS INTERFACE port=vid_in axis
#pragma HLS INTERFACE port=vid_out axis
#pragma HLS stream variable=vid_in
#pragma HLS stream variable=vid_out

	#pragma HLS INTERFACE s_axilite port=copy_shadow 				offset=0x0080		bundle=BUS_CTRL
	#pragma HLS INTERFACE s_axilite port=colormatrix 				offset=0x0010		bundle=BUS_CTRL

	#pragma HLS array_partition variable=colormatrix complete
	static Q colormatrix_local[12] = { 64, 0, 0, 0,
			                            0, 64, 0, 0,
										0, 0, 64, 0};

#pragma HLS array_partition variable=colormatrix_local complete
	if (copy_shadow) {
		for(int i=0;i<12;i++) colormatrix_local[i] = colormatrix[i];
	}

	axiPixelIn pixIn = vid_in.read();
	axiPixelOut pixOut;
	pixOut.keep = pixIn.keep;
	pixOut.strb = pixIn.strb;
	pixOut.dest = pixIn.dest;
	pixOut.last = 0;
	pixOut.user = pixIn.user;

	pixOut.data = color_mul(pixIn.data.range(23, 0), colormatrix);
	vid_out.write(pixOut);
	axiPixelOut pixOut2;
	pixOut2.keep = pixIn.keep;
	pixOut2.strb = pixIn.strb;
	pixOut2.dest = pixIn.dest;
	pixOut2.last = pixIn.last;
	pixOut2.user = 0;
	pixOut2.data = color_mul(pixIn.data.range(47, 24), colormatrix);
	vid_out.write(pixOut2);
}
