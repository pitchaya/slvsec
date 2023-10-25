#include "slvsrx_lanealigner.h"
#include <hls_stream.h>
#include <ap_int.h>

const ap_uint<8> PADCHAR0("f7", 16);
const ap_uint<8> PADCHAR1("9c", 16);
const ap_uint<8> PADCHAR2("dc", 16);
const ap_uint<8> PADCHAR3("7c", 16);

bool isPad(ap_uint<8> inp) {
	return (inp == PADCHAR0 || inp == PADCHAR1 || inp == PADCHAR2
			|| inp == PADCHAR3);
}

template <int an_unused_template_parameter>
void pad_removal(hls::stream<axiWord> &inData, hls::stream<axiWord> &outData) {

#pragma HLS PIPELINE II=1 enable_flush
	static int padPos = 0;
	static axiWord padRemoveData;
//#pragma HLS stream depth=4 variable=padRemoveInData


	static hls::stream<ap_uint<40> > sbuf0_aligned;
#pragma HLS stream depth=8 variable=sbuf0_aligned

	static hls::stream<ap_uint<40> > sbuf0;
#pragma HLS stream depth=8 variable=sbuf0

	static hls::stream<ap_uint<40> > sbuf1;
#pragma HLS stream depth=8 variable=sbuf1

	static hls::stream<ap_uint<40> > sbuf2;
#pragma HLS stream depth=8 variable=sbuf2
	axiread:

	if (!inData.empty()) {
//	#pragma HLS PIPELINE II=1

		axiWord newInput = inData.read();
		ap_uint<4> valids_p0 = 0;
		ap_uint<32> dataword_p0 = 0;
		ap_uint<4> charisk_p0 = 0;

		valids_p0[0] = !(newInput.data.get_bit(32)
				&& isPad(newInput.data.range(7, 0)));
		valids_p0[1] = !(newInput.data.get_bit(33)
				&& isPad(newInput.data.range(15, 8)));
		valids_p0[2] = !(newInput.data.get_bit(34)
				&& isPad(newInput.data.range(23, 16)));
		valids_p0[3] = !(newInput.data.get_bit(35)
				&& isPad(newInput.data.range(31, 24)));

		dataword_p0 = newInput.data.range(31, 0);
		charisk_p0 = newInput.data.range(35, 32);

		sbuf0_aligned.write((valids_p0, charisk_p0, dataword_p0));

#if PRINT
		std::cout << "0:" << std::hex << std::setw(4) << valids_p0 << " " << std::setw(12) << dataword_p0 << " ";//<< std::endl;
		std::cout << "1:" << std::hex << std::setw(4) << valids_p1 << " " << std::setw(12) << dataword_p1 << " ";//<< std::endl;
		std::cout << "2:" << std::hex << std::setw(4) << valids_p2 << " " << std::setw(12) << dataword_p2 << " ";// << std::endl;
		std::cout << "3:" << std::hex << std::setw(4) << valids_p3 << " " << std::setw(12) << dataword_p3 << " ";// << std::endl;
		std::cout << "4:" << std::hex << std::setw(4) << valids_p4 << " " << std::setw(12) << dataword_p4 << "\n";// << std::endl;
#endif
	}

	if (!sbuf0_aligned.empty()) {

		ap_uint<40> vals = sbuf0_aligned.read();
		int shf = 0;
		if (vals[37] && !vals[36])
			shf = 1;
		if (vals[38] && !vals[37])
			shf = 2;
		if (vals[39] && !vals[38])
			shf = 3;

		ap_uint<4> left_valids = 0;
		ap_uint<4> left_charisk = 0;
		ap_uint<32> left_dataword = 0;

		left_valids = vals.range(39, 36) >> shf;
		left_charisk = vals.range(35, 32) >> shf;
		left_dataword = vals.range(31, 0) >> (shf * 8);
		// something is valid here, just pass to the queue

		if (vals.range(39, 36) != 0)
			sbuf0.write((left_valids, left_charisk, left_dataword));
	}

	if (!sbuf0.empty()) {
		ap_uint<4> valids = 0;
		ap_uint<4> charisk = 0;
		ap_uint<32> dataword = 0;
		ap_uint<4> ovalids = 0;
		ap_uint<40> outdata = 0;

		static ap_uint<4> l_valids = 0;
		static ap_uint<4> l_charisk = 0;
		static ap_uint<32> l_dataword = 0;

		(valids, charisk, dataword) = sbuf0.read();

		if (l_valids[0] && !l_valids[1]) {
			ovalids = (valids.range(2, 0), l_valids.range(0, 0));
			outdata =
					(ovalids, charisk.range(2, 0), l_charisk.range(0, 0), dataword.range(
							23, 0), l_dataword.range(7, 0));
			l_valids = valids.range(3, 3);
			l_charisk = charisk.range(3, 3);
			l_dataword = dataword.range(31, 24);

		} else {
			ovalids = l_valids;
			outdata = (l_valids, l_charisk, l_dataword);

			l_valids = valids;
			l_charisk = charisk;
			l_dataword = dataword;
		}
		if (ovalids != 0) {
			sbuf1.write(outdata);
		}
	}

	if (!sbuf1.empty()) {
		ap_uint<4> valids = 0;
		ap_uint<4> charisk = 0;
		ap_uint<32> dataword = 0;
		axiWord outD = { 0, 0 };
		ap_uint<4> ovalids = 0;
		static ap_uint<4> l_valids = 0;
		static ap_uint<4> l_charisk = 0;
		static ap_uint<32> l_dataword = 0;

		(valids, charisk, dataword) = sbuf1.read();
		if (l_valids[0] && l_valids[1] && !l_valids[2]) {
			outD.data.range(35, 32) = (charisk.range(1, 0), l_charisk.range(1,
					0));
			outD.data.range(31, 0) = (dataword.range(15, 0), l_dataword.range(
					15, 0));
			ovalids = (valids.range(1, 0), l_valids.range(1, 0));

			l_valids = valids.range(3, 2);
			l_charisk = charisk.range(3, 2);
			l_dataword = dataword.range(31, 16);

		} else {
			outD.data.range(35, 32) = l_charisk;
			outD.data.range(31, 0) = l_dataword;
			ovalids = l_valids;

			l_valids = valids;
			l_charisk = charisk;
			l_dataword = dataword;
		}
		if (ovalids != 0) {
			outData.write(outD);
		}
	}

}

const ap_uint<36> IDLE_CODE("000000000", 16);
const ap_uint<36> SYNC_CODE("fbcbcbcbc", 16);  // this is K.28.5
const ap_uint<36> START_CODE("ffb5cfbbc", 16); // This is K.28.5 K27.7 K.28.2, K27.7
const ap_uint<36> STOP_CODE("ffdfefdbc", 16); // This is K.28.5 K29.7 K.30.7, K29.7

template <int an_unused_template_parameter>
void slvsrx_lanealigner_i(hls::stream<axiWord> &inData,
		hls::stream<axiWord> &outData) {

#pragma HLS pipeline II=1 enable_flush

#pragma HLS INTERFACE port=inData axis
#pragma HLS INTERFACE port=outData axis

	static enum dState {
		D_IDLE = 0, D_HEADER, D_STREAM, D_END1, D_END2
	} rxState = D_IDLE;


	static enum pState {
		PART0 = 0, PART1, PART2, PART3
	} pState = PART0;
	//static axiWord lastWord = {0,0};
	static ap_uint<64> dataword = 0;
	static ap_uint<8> charisk = 0;

//	static bool nextEnd = false;

	if (!inData.empty()) {
		axiWord newInput = inData.read();
		// Right shift barrel
		dataword.range(31, 0) = dataword.range(63, 32);
		dataword.range(63, 32) = newInput.data.range(31, 0);

		charisk.range(3, 0) = charisk.range(7, 4);
		charisk.range(7, 4) = newInput.data.range(35, 32);

		static axiWord sendWord = { 0, 0 };

		// This is very hard code for a very specific case, again, writing this in HDL would be much much simpler. maybe I should do that.
		switch (rxState) {
		case D_IDLE:
			sendWord.data = 0;
			sendWord.last = 0;
			if (charisk.range(3, 0) == 0xF
					&& dataword.range(31, 0) == START_CODE.range(31, 0)) {
				pState = PART0;
				rxState = D_HEADER;
				sendWord.data.range(31, 0) = dataword.range(31, 0);
				sendWord.data.range(35, 32) = charisk.range(3, 0);
			}
			if (charisk.range(4, 1) == 0xF
					&& dataword.range(31 + 8, 0 + 8)
							== START_CODE.range(31, 0)) {
				pState = PART1;
				rxState = D_HEADER;
				sendWord.data.range(31, 0) = dataword.range(31 + 8, 0 + 8);
				sendWord.data.range(35, 32) = charisk.range(3 + 1, 0 + 1);
			}
			if (charisk.range(5, 2) == 0xF
					&& dataword.range(31 + 16, 0 + 16)
							== START_CODE.range(31, 0)) {
				pState = PART2;
				rxState = D_HEADER;
				sendWord.data.range(31, 0) = dataword.range(31 + 16, 0 + 16);
				sendWord.data.range(35, 32) = charisk.range(3 + 2, 0 + 2);
			}
			if (charisk.range(6, 3) == 0xF
					&& dataword.range(31 + 24, 0 + 24)
							== START_CODE.range(31, 0)) {
				pState = PART3;
				rxState = D_HEADER;
				sendWord.data.range(31, 0) = dataword.range(31 + 24, 0 + 24);
				sendWord.data.range(35, 32) = charisk.range(3 + 3, 0 + 3);
			}
			break;
		case D_HEADER:
		case D_STREAM:
			if (rxState == D_STREAM || rxState == D_HEADER) {
				outData.write(sendWord);
			}
			switch (pState) {
			case PART0:
				sendWord.data.range(31, 0) = dataword.range(31, 0);
				sendWord.data.range(35, 32) = charisk.range(3, 0);
				break;
			case PART1:
				sendWord.data.range(31, 0) = dataword.range(31 + 8, 0 + 8);
				sendWord.data.range(35, 32) = charisk.range(3 + 1, 0 + 1);
				break;
			case PART2:
				sendWord.data.range(31, 0) = dataword.range(31 + 16, 0 + 16);
				sendWord.data.range(35, 32) = charisk.range(3 + 2, 0 + 2);
				break;
			case PART3:
				sendWord.data.range(31, 0) = dataword.range(31 + 24, 0 + 24);
				sendWord.data.range(35, 32) = charisk.range(3 + 3, 0 + 3);
				break;
			}
			rxState = D_STREAM;
#if 1
			if (charisk.range(3, 0) == 0xF
					&& dataword.range(31, 0) == STOP_CODE.range(31, 0)) {
				// This case is a bit impossible if pState is not PART0, but in general, just make it the last state
				sendWord.last = 1;
				rxState = D_END2;
			}
			if (charisk.range(4, 1) == 0xF
					&& dataword.range(31 + 8, 0 + 8)
							== STOP_CODE.range(31, 0)) {
				if (pState == PART0)
					rxState = D_END1;
				if (pState == PART1) {
					sendWord.last = 1;
					rxState = D_END2;
				}
			}
			if (charisk.range(5, 2) == 0xF
					&& dataword.range(31 + 16, 0 + 16)
							== STOP_CODE.range(31, 0)) {
				rxState = D_END1;
				if (pState == PART2) {
					sendWord.last = 1;
					rxState = D_END2;
				}

			}
			if (charisk.range(6, 3) == 0xF
					&& dataword.range(31 + 24, 0 + 24)
							== STOP_CODE.range(31, 0)) {
				rxState = D_END1;
				if (pState == PART3){
					sendWord.last = 1;
					rxState = D_END2;
				}
			}

			break;

		case D_END1:
//			std::cout << std::hex << sendWord.data << std::endl;
			outData.write(sendWord);
			sendWord.data.range(31,0) = STOP_CODE;
			sendWord.data.range(35,32) = 0xF;
			sendWord.last = 1;
			rxState = D_END2;
			break;
		case D_END2:
//			std::cout << std::hex << sendWord.data << std::endl;
			outData.write(sendWord);
			sendWord.last = 0;
			rxState = D_IDLE;
			break;
//			sendWord.data.range(31,0) = STOP_CODE;
//			sendWord.data.range(35,32) = 0xF;
//			sendWord.last = 1;
#endif
#if 0
			if (charisk.range(3, 0) == 0xF
					&& dataword.range(31, 0) == STOP_CODE.range(31, 0)) {
				rxState = D_END;
				sendWord.data.range(31,0) = STOP_CODE;
				sendWord.data.range(35,32) = 0xF;
				sendWord.last = 1;
//				sendWord.data.range(31, 0) = dataword.range(31, 0);
//				sendWord.data.range(35, 32) = charisk.range(3, 0);
			}
			if (charisk.range(4, 1) == 0xF
					&& dataword.range(31 + 8, 0 + 8)
							== STOP_CODE.range(31, 0)) {
				rxState = D_END;
				sendWord.data.range(31,0) = STOP_CODE;
				sendWord.data.range(35,32) = 0xF;
				sendWord.last = 1;

//				sendWord.data.range(31, 0) = dataword.range(31 + 8, 0 + 8);
//				sendWord.data.range(35, 32) = charisk.range(3 + 1, 0 + 1);
			}
			if (charisk.range(5, 2) == 0xF
					&& dataword.range(31 + 16, 0 + 16)
							== STOP_CODE.range(31, 0)) {
				rxState = D_END;
				sendWord.data.range(31,0) = STOP_CODE;
				sendWord.data.range(35,32) = 0xF;
				sendWord.last = 1;
//				sendWord.data.range(31, 0) = dataword.range(31 + 16, 0 + 16);
//				sendWord.data.range(35, 32) = charisk.range(3 + 2, 0 + 2);
			}
			if (charisk.range(6, 3) == 0xF
					&& dataword.range(31 + 24, 0 + 24)
							== STOP_CODE.range(31, 0)) {
				rxState = D_END;
				sendWord.data.range(31,0) = STOP_CODE;
				sendWord.data.range(35,32) = 0xF;
				sendWord.last = 1;
//				sendWord.data.range(31, 0) = dataword.range(31 + 24, 0 + 24);
//				sendWord.data.range(35, 32) = charisk.range(3 + 3, 0 + 3);
			}

			break;
		case D_END:
			outData.write(sendWord);
			sendWord.last = 0;

			rxState = D_IDLE;
#endif
			break;
		};
	}

	// Always writing an output
//	outData.write(currWord);

}

template <int an_unused_template_parameter>
void slvsrx_lanealigner_t(hls::stream<axiWord> &inData,
		hls::stream<axiWord> &outData) {
#pragma HLS DATAFLOW
//#pragma HLS PIPELINE II=1 enable_flush
//#pragma HLS INTERFACE port=inData axis
//#pragma HLS INTERFACE port=outData axis

	static hls::stream<axiWord> padRemoveInData;
#pragma HLS stream depth=4 variable=padRemoveInData
//	pad_removal<an_unused_template_parameter>(inData, outData);

	pad_removal<an_unused_template_parameter>(inData, padRemoveInData);
	slvsrx_lanealigner_i<an_unused_template_parameter>(padRemoveInData, outData);

//	slvsrx_lanealigner_i(inData, outData);
}


void slvsrx_lanealigner(hls::stream<axiWord> &inData,
		hls::stream<axiWord> &outData) {
#pragma HLS DATAFLOW
//#pragma HLS PIPELINE II=1 enable_flush
#pragma HLS INTERFACE port=inData axis
#pragma HLS INTERFACE port=outData axis
	slvsrx_lanealigner_t<0>(inData, outData);


}

#if 0
void slvsrx_merge_i(hls::stream<axiWord> &inData0,
			hls::stream<axiWord> &inData1,
			hls::stream<axiWord> &inData2,
			hls::stream<axiWord> &inData3,
			hls::stream<axiPixel> &outData,
			bool *framestart,
			bool *frameend,
			bool *linevalid,
			ap_uint<16> *curline) {

	static axiWord idat[4];
	static ap_uint<8> end_count = 0;
	static bool readNewWord[4] = {true, true, true, true};
	static bool isLast[4] = {false, false, false, false};
	bool eqSTART[4] = {false, false, false};

#pragma HLS array_partition variable=readNewWord complete
#pragma HLS array_partition variable=isLast complete
#pragma HLS array_partition variable=eqSTART complete


#pragma HLS pipeline II=1 enable_flush
	static enum dState {
		D_IDLE = 0, D_HEADER, D_STREAM, D_END
	} rxState = D_IDLE;

	const ap_uint<36> IDLE_CODE("000000000", 16);
	const ap_uint<36>  SYNC_CODE("fbcbcbcbc", 16);  // this is K.28.5
	const ap_uint<36> START_CODE("ffb5cfbbc", 16); // This is K.28.5 K27.7 K.28.2, K27.7
	const ap_uint<36>  STOP_CODE("ffdfefdbc", 16); // This is K.28.5 K29.7 K.30.7, K29.7

	(*curline).range(2,0) = rxState;

	// I'm a bit bad about padding, it's going to bite me later.... watch it.
	static axiPixel pixel = {0, 0};
	// Everybody is ready, eh?

#if 0
	if (!inData0.empty())	idat[0] = inData0.read();
	if (!inData1.empty())	idat[1] = inData1.read();
	if (!inData2.empty())	idat[2] = inData2.read();
	if (!inData3.empty())	idat[3] = inData3.read();

	ap_uint<128> comdat = (
			  idat[0].data.range(31,24),  idat[1].data.range(31,24), idat[2].data.range(31,24), idat[3].data.range(31,24),
			  idat[0].data.range(23,16),  idat[1].data.range(23,16), idat[2].data.range(23,16), idat[3].data.range(23,16),
			  idat[0].data.range(15, 8),  idat[1].data.range(15, 8), idat[2].data.range(15, 8), idat[3].data.range(15, 8),
			  idat[0].data.range( 7, 0),  idat[1].data.range( 7, 0), idat[2].data.range( 7, 0), idat[3].data.range( 7, 0));
	pixel.data = comdat;
	pixel.last = 0;
	outData.write(pixel);

	if (readNewWord[0]) {
		if (!inData0.empty()) {
			idat[0] = inData0.read();
		}
	}
	if (readNewWord[1]) {
		if (!inData1.empty()) {
			idat[1] = inData0.read();
		}
	}
	if (readNewWord[2]) {
		if (!inData2.empty()) {
			idat[2] = inData0.read();
		}
	}
	if (readNewWord[3]) {
		if (!inData3.empty()) {
			idat[3] = inData0.read();
		}
	}

#endif
#if 1

//	printf("All have data: %d\n", !inData0.empty() + !inData1.empty()  + ! inData2.empty() + !inData3.empty());
	if (!inData0.empty() && !inData1.empty()  && ! inData2.empty() && !inData3.empty()) {
//		if (readNewWord[0])  idat[0] = inData0.read();
//		if (readNewWord[1])  idat[1] = inData1.read();
//		if (readNewWord[2])  idat[2] = inData2.read();
//		if (readNewWord[3])  idat[3] = inData3.read();
		idat[0] = inData0.read();
		idat[1] = inData1.read();
		idat[2] = inData2.read();
		idat[3] = inData3.read();

//		for(int k=0;k<4;k++)
//			eqSTART[k] =  idat[k].data == START_CODE;

//		eqSTART[0] =  idat[0].data == START_CODE;
//		eqSTART[1] =  idat[1].data == START_CODE;
//		eqSTART[2] =  idat[2].data == START_CODE;
//		eqSTART[3] =  idat[3].data == START_CODE;
		eqSTART[0] = eqSTART[1] = eqSTART[2] = eqSTART[3] = true;

		ap_uint<128> comdat = (
				  idat[0].data.range(31,24),  idat[1].data.range(31,24), idat[2].data.range(31,24), idat[3].data.range(31,24),
				  idat[0].data.range(23,16),  idat[1].data.range(23,16), idat[2].data.range(23,16), idat[3].data.range(23,16),
				  idat[0].data.range(15, 8),  idat[1].data.range(15, 8), idat[2].data.range(15, 8), idat[3].data.range(15, 8),
				  idat[0].data.range( 7, 0),  idat[1].data.range( 7, 0), idat[2].data.range( 7, 0), idat[3].data.range( 7, 0));
		ap_uint<32> header =  (idat[0].data.range( 7, 0), idat[1].data.range( 7, 0), idat[2].data.range( 7, 0), idat[4].data.range( 7, 0),
						       idat[0].data.range(15, 8), idat[1].data.range(15, 8),idat[2].data.range(15, 8), idat[3].data.range(15, 8));
		switch (rxState) {
			case D_IDLE:
				if (eqSTART[0] && eqSTART[1] && eqSTART[2] && eqSTART[3]) {
					rxState = D_HEADER;
				} else {
					for(int k=0;k<4;k++)
						readNewWord[k] = !eqSTART[k];
				}
				break;
			case D_HEADER:

				*framestart = comdat[31];
				  *frameend = comdat[30];
				  *linevalid = comdat[29];
				  (*curline).range(15,3) = comdat.range(28, 16);

				  rxState = D_STREAM;

				pixel.data = comdat;
				pixel.last = 0;
				break;
			case D_STREAM:
				outData.write(pixel);
				pixel.data = comdat;
				pixel.last = 0;
				if (idat[0].last || idat[1].last ||	idat[2].last || idat[3].last) {
//					pixel.data = comdat;
					for(int k=0;k<4;k++) isLast[k] = idat[k].last;
					rxState = D_END;
					pixel.last = 1;
				}
				break;
			case D_END:
				outData.write(pixel);
				end_count = 0;
//				rxState = D_IDLE;
				break;
		};
	} else {
		// This is a special case
		if (rxState == D_END) {
			// expired after 32 clocks cycle, just go back, yeah, this might misalign
			if ((isLast[0] && isLast[1] && isLast[2] && isLast[3]) || (end_count[6]) )
				rxState = D_IDLE;
			end_count ++;
			if (!isLast[0])	if (!inData0.empty())  isLast[0] = inData0.read().last;
			if (!isLast[1])	if (!inData1.empty())  isLast[1] = inData1.read().last;
			if (!isLast[2])	if (!inData2.empty())  isLast[2] = inData2.read().last;
			if (!isLast[3])	if (!inData3.empty())  isLast[3] = inData3.read().last;
		}
	}
#endif



}

#endif

void slvsrx_merge_i2(//hls::stream<axiWord> &inData[4],
		hls::stream<axiWord> &inData0,
				hls::stream<axiWord> &inData1,
				hls::stream<axiWord> &inData2,
				hls::stream<axiWord> &inData3,
				hls::stream<axiPixel> &outData,
			bool *framestart,
			bool *frameend,
			bool *linevalid,
			ap_uint<16> *curline,
			ap_uint<16> *lastPacketSize) {


	static ap_uint<8> end_count = 0;
	//ap_uint<16> *lastPacketSize = 0;
	static ap_uint<16> lastPacketSizeCnt = 0;
	static bool readNewWord[4] = {true, true, true, true};
	static bool isLast[4] = {false, false, false, false};
	static bool eqSTART[4] = {false, false, false, false};
//	static bool newData[4] = {false, false, false, false};

//#pragma HLS array_partition variable=idat complete
#pragma HLS array_partition variable=readNewWord complete
#pragma HLS array_partition variable=isLast complete
#pragma HLS array_partition variable=eqSTART complete

#pragma HLS INLINE off
#pragma HLS pipeline II=1 enable_flush
	static enum dState {
		D_IDLE = 0, D_HEADER, D_STREAM, D_END, D_DISCARD
	} rxState = D_IDLE;

	const ap_uint<36> IDLE_CODE("000000000", 16);
	const ap_uint<36>  SYNC_CODE("fbcbcbcbc", 16);  // this is K.28.5
	const ap_uint<36> START_CODE("ffb5cfbbc", 16); // This is K.28.5 K27.7 K.28.2, K27.7
	const ap_uint<36>  STOP_CODE("ffdfefdbc", 16); // This is K.28.5 K29.7 K.30.7, K29.7

	//(*curline).range(2,0) = rxState;

	// I'm a bit bad about padding, it's going to bite me later.... watch it.
	static axiPixel pixel = {0, 0};
	// Everybody is ready, eh?
#if 0
	if (!inData0.empty()) {	if (readNewWord[0]) { idat[0] = inData0.read(); 	eqSTART[0] =  idat[0].data == START_CODE; } }
	if (!inData1.empty()) {
	if (readNewWord[1]) {
			idat[1] = inData0.read();
			eqSTART[1] =  idat[1].data == START_CODE;

		}
	}
	if (!inData2.empty()) {
	if (readNewWord[2]) {

			idat[2] = inData0.read();
			eqSTART[2] =  idat[2].data == START_CODE;

		}
	}
	if (!inData3.empty()) {
	if (readNewWord[3]) {

			idat[3] = inData0.read();
			eqSTART[3] =  idat[3].data == START_CODE;
		}
	}
#endif
//	if (!inData0.empty() {
//		if (readNewWord[0])  idat[0] = inData0.read();
//	}
	bool streamEmpty[4];
//	for(uint8_t i=0;i<4;i++)
//		streamEmpty[i] =inData[i].empty();
	streamEmpty[0] = inData0.empty();
	streamEmpty[1] = inData1.empty();
	streamEmpty[2] = inData2.empty();
	streamEmpty[3] = inData3.empty();
	static bool first = true;
	if (first) {
		*framestart = 0;
		*frameend = 0;
		*linevalid = 0;
		*curline = 0;
		*lastPacketSize = 0;
		first = false;

	}
	pixel.user = 0;
	switch (rxState) {
		case D_IDLE:
		{
			static axiWord idat[4];
#pragma HLS ARRAY_PARTITION variable=streamEmpty complete
			if (!streamEmpty[0]) {	if (readNewWord[0]) { idat[0] = inData0.read(); 	eqSTART[0] =  idat[0].data == START_CODE; readNewWord[0] = !eqSTART[0]; } }
			if (!streamEmpty[1]) {	if (readNewWord[1]) { idat[1] = inData1.read(); 	eqSTART[1] =  idat[1].data == START_CODE; readNewWord[1] = !eqSTART[1]; } }
			if (!streamEmpty[2]) {	if (readNewWord[2]) { idat[2] = inData2.read(); 	eqSTART[2] =  idat[2].data == START_CODE; readNewWord[2] = !eqSTART[2]; } }
			if (!streamEmpty[3]) {	if (readNewWord[3]) { idat[3] = inData3.read(); 	eqSTART[3] =  idat[3].data == START_CODE; readNewWord[3] = !eqSTART[3]; } }
#if PRINT
			std::cout << "IDLE\n";
			for(int i=0;i<4;i++)
				std::cout << i << " " << std::hex << streamEmpty[i] << " " << readNewWord[i] << idat[i].data << std::endl;
#endif

			if (eqSTART[0] && eqSTART[1] && eqSTART[2] && eqSTART[3]) {
				rxState = D_HEADER;
				for(int k=0;k<4;k++)
					readNewWord[k] = true;
			}
//			ap_uint<128> comdat = (
//					  idat[0].data.range(31,24),  idat[1].data.range(31,24), idat[2].data.range(31,24), idat[3].data.range(31,24),
//					  idat[0].data.range(23,16),  idat[1].data.range(23,16), idat[2].data.range(23,16), idat[3].data.range(23,16),
//					  idat[0].data.range(15, 8),  idat[1].data.range(15, 8), idat[2].data.range(15, 8), idat[3].data.range(15, 8),
//					  idat[0].data.range( 7, 0),  idat[1].data.range( 7, 0), idat[2].data.range( 7, 0), idat[3].data.range( 7, 0));

			ap_uint<128> comdat = (
					  idat[3].data.range(31,24),  idat[2].data.range(31,24), idat[1].data.range(31,24), idat[0].data.range(31,24),
					  idat[3].data.range(23,16),  idat[2].data.range(23,16), idat[1].data.range(23,16), idat[0].data.range(23,16),
					  idat[3].data.range(15, 8),  idat[2].data.range(15, 8), idat[1].data.range(15, 8), idat[0].data.range(15, 8),
					  idat[3].data.range( 7, 0),  idat[2].data.range( 7, 0), idat[1].data.range( 7, 0), idat[0].data.range( 7, 0));


			lastPacketSizeCnt = 0;
			pixel.data = comdat;
			pixel.keep = 0xffff;
			pixel.strb = 0xffff;
			pixel.user = 0;
			pixel.last = 0;
			break;
		}
		case D_HEADER:
		{
			axiWord idat[4];
//			std::cout << "HEADER\n";
//			for(int i=0;i<4;i++)
//				std::cout << i << " " << std::hex << streamEmpty[i] << " " << readNewWord[i] << idat[i].data << std::endl;

#pragma HLS ARRAY_PARTITION variable=streamEmpty complete
			if (!streamEmpty[0] && !streamEmpty[1] && !streamEmpty[2] && !streamEmpty[3]) {
				// Always read
				if (!streamEmpty[0]) {	idat[0] = inData0.read();  }
				if (!streamEmpty[1]) {	idat[1] = inData1.read();  }
				if (!streamEmpty[2]) {	idat[2] = inData2.read();  }
				if (!streamEmpty[3]) {	idat[3] = inData3.read();  }
#if PRINT
				std::cout << "HEADER\n";
				for(int i=0;i<4;i++)
					std::cout << i << " " << std::hex << streamEmpty[i] << " " << readNewWord[i] << idat[i].data << std::endl;
#endif

				ap_uint<128> comdat_old = (
						  idat[0].data.range(31,24),  idat[1].data.range(31,24), idat[2].data.range(31,24), idat[3].data.range(31,24),
						  idat[0].data.range(23,16),  idat[1].data.range(23,16), idat[2].data.range(23,16), idat[3].data.range(23,16),
						  idat[0].data.range(15, 8),  idat[1].data.range(15, 8), idat[2].data.range(15, 8), idat[3].data.range(15, 8),
						  idat[0].data.range( 7, 0),  idat[1].data.range( 7, 0), idat[2].data.range( 7, 0), idat[3].data.range( 7, 0));
				ap_uint<128> comdat = (
						  idat[3].data.range(31,24),  idat[2].data.range(31,24), idat[1].data.range(31,24), idat[0].data.range(31,24),
						  idat[3].data.range(23,16),  idat[2].data.range(23,16), idat[1].data.range(23,16), idat[0].data.range(23,16),
						  idat[3].data.range(15, 8),  idat[2].data.range(15, 8), idat[1].data.range(15, 8), idat[0].data.range(15, 8),
						  idat[3].data.range( 7, 0),  idat[2].data.range( 7, 0), idat[1].data.range( 7, 0), idat[0].data.range( 7, 0));

	//			ap_uint<32> header =  (idat[0].data.range( 7, 0), idat[1].data.range( 7, 0), idat[2].data.range( 7, 0), idat[4].data.range( 7, 0),
	//								   idat[0].data.range(15, 8), idat[1].data.range(15, 8),idat[2].data.range(15, 8), idat[3].data.range(15, 8));
				// I'm keeping this only because I'm tooo lazy
				*framestart = comdat_old[31];
				*frameend = comdat_old[30];
				*linevalid = comdat_old[29];
				(*curline) = comdat_old.range(28, 16);
				pixel.user = comdat_old[31];

				rxState = D_STREAM;
				// This is a bit tricky, but can we do it?

				lastPacketSizeCnt++;
				outData.write(pixel);

				pixel.data = comdat;
				pixel.last = 0;
				// Frame start here !!


			}

			break;
		}
		case D_STREAM:
		{
			axiWord idat[4];
#pragma HLS ARRAY_PARTITION variable=streamEmpty complete
			if (!streamEmpty[0] && !streamEmpty[1] && !streamEmpty[2] && !streamEmpty[3]) {
				if (!streamEmpty[0]) {	idat[0] = inData0.read();  }
				if (!streamEmpty[1]) {	idat[1] = inData1.read();  }
				if (!streamEmpty[2]) {	idat[2] = inData2.read();  }
				if (!streamEmpty[3]) {	idat[3] = inData3.read();  }
//				ap_uint<128> comdat = (
//						  idat[0].data.range(31,24),  idat[1].data.range(31,24), idat[2].data.range(31,24), idat[3].data.range(31,24),
//						  idat[0].data.range(23,16),  idat[1].data.range(23,16), idat[2].data.range(23,16), idat[3].data.range(23,16),
//						  idat[0].data.range(15, 8),  idat[1].data.range(15, 8), idat[2].data.range(15, 8), idat[3].data.range(15, 8),
//						  idat[0].data.range( 7, 0),  idat[1].data.range( 7, 0), idat[2].data.range( 7, 0), idat[3].data.range( 7, 0));
				ap_uint<128> comdat = (
						  idat[3].data.range(31,24),  idat[2].data.range(31,24), idat[1].data.range(31,24), idat[0].data.range(31,24),
						  idat[3].data.range(23,16),  idat[2].data.range(23,16), idat[1].data.range(23,16), idat[0].data.range(23,16),
						  idat[3].data.range(15, 8),  idat[2].data.range(15, 8), idat[1].data.range(15, 8), idat[0].data.range(15, 8),
						  idat[3].data.range( 7, 0),  idat[2].data.range( 7, 0), idat[1].data.range( 7, 0), idat[0].data.range( 7, 0));

//				ap_uint<32> header =  (idat[0].data.range( 7, 0), idat[1].data.range( 7, 0), idat[2].data.range( 7, 0), idat[4].data.range( 7, 0),
//									   idat[0].data.range(15, 8), idat[1].data.range(15, 8),idat[2].data.range(15, 8), idat[3].data.range(15, 8));

				pixel.user = 0;
				lastPacketSizeCnt++;
				outData.write(pixel);
				pixel.data = comdat;
				//pixel.user = 0;
				pixel.last = 0;
			// After last, I assume that there is at least a few beat before new data
			// If it's the last, don't read in the next state
				if (idat[0].last || idat[1].last ||	idat[2].last || idat[3].last) {
	//					pixel.data = comdat;
//					std::cout << idat[0].last  << idat[1].last << idat[2].last  << idat[3].last << std::endl;
//					std::cout << idat[0].data  << idat[1].data << idat[2].data  << idat[3].data << std::endl;
					for(int k=0;k<4;k++) {
						// Stop reading on all, if we read, then we might discard things
						readNewWord[k] = false; // !idat[k].last;
						isLast[k] = idat[k].last;
					}
					rxState = D_END;
					pixel.last = 1;
				}
			} else {
				pixel.user = 0;
			}
			break;
		}

		case D_END:
		{
			lastPacketSizeCnt++;
			outData.write(pixel);
			end_count = 0;
			// Ok, we need to read data
			for(int k=0;k<4;k++)readNewWord[k] = true;
			for(int k=0;k<4;k++)    eqSTART[k] = false;
			*lastPacketSize = lastPacketSizeCnt;
			rxState = D_IDLE;
		}
			break;
		case D_DISCARD:

			break;
	};

/*	} else {
		// This is a special case
		if (rxState == D_END) {
			// expired after 32 clocks cycle, just go back, yeah, this might misalign
//			if ((isLast[0] && isLast[1] && isLast[2] && isLast[3]) || (end_count[6]) )
			if (end_count[6])
				rxState = D_IDLE;
			end_count ++;
//			if (!isLast[0])	if (!inData0.empty())  isLast[0] = inData0.read().last;
//			if (!isLast[1])	if (!inData1.empty())  isLast[1] = inData1.read().last;
//			if (!isLast[2])	if (!inData2.empty())  isLast[2] = inData2.read().last;
//			if (!isLast[3])	if (!inData3.empty())  isLast[3] = inData3.read().last;
		}
	}*/
}

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
		) {
#pragma HLS DATAFLOW
//#pragma HLS PIPELINE II=1 enable_flush
#pragma HLS INTERFACE port=inData0 axis
#pragma HLS INTERFACE port=inData1 axis
#pragma HLS INTERFACE port=inData2 axis
#pragma HLS INTERFACE port=inData3 axis

#pragma HLS INTERFACE port=outData axis

	#pragma HLS INTERFACE ap_ovld port=framestart
//#pragma HLS INTERFACE ap_none port=frameend
//#pragma HLS INTERFACE ap_none port=linevalid
//#pragma HLS INTERFACE ap_none port=curline
//#pragma HLS INTERFACE ap_none port=lastPacketSize
//#pragma HLS INTERFACE ap_ovld register port=frameend
//#pragma HLS INTERFACE ap_ovld register port=linevalid
//#pragma HLS INTERFACE ap_ovld register port=curline
//#pragma HLS INTERFACE ap_ovld register port=lastPacketSize
#pragma HLS INTERFACE ap_ovld port=frameend
#pragma HLS INTERFACE ap_ovld port=linevalid
#pragma HLS INTERFACE ap_ovld port=curline
#pragma HLS INTERFACE ap_ovld port=lastPacketSize



#if 0
	static hls::stream<axiWord> internalData0;
	static hls::stream<axiWord> internalData1;
	static hls::stream<axiWord> internalData2;
	static hls::stream<axiWord> internalData3;

#pragma HLS stream depth=16 variable=internalData0
#pragma HLS stream depth=16 variable=internalData1
#pragma HLS stream depth=16 variable=internalData2
#pragma HLS stream depth=16 variable=internalData3

	slvsrx_lanealigner_t<0>(inData0, internalData0);
	slvsrx_lanealigner_t<1>(inData1, internalData1);
	slvsrx_lanealigner_t<2>(inData2, internalData2);
	slvsrx_lanealigner_t<3>(inData3, internalData3);

	slvsrx_merge_i2(internalData0, internalData1, internalData2, internalData3, outData, framestart, frameend, linevalid, curline);
#else
	slvsrx_merge_i2(inData0,
			inData1, inData2, inData3, outData, framestart, frameend, linevalid, curline, lastPacketSize);
#endif
}


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
		) {
#pragma HLS DATAFLOW
//#pragma HLS PIPELINE II=1 enable_flush
#pragma HLS INTERFACE port=inData0 axis
#pragma HLS INTERFACE port=inData1 axis
#pragma HLS INTERFACE port=inData2 axis
#pragma HLS INTERFACE port=inData3 axis

#pragma HLS INTERFACE port=outData axis
	static hls::stream<axiWord> internalData0;
	static hls::stream<axiWord> internalData1;
	static hls::stream<axiWord> internalData2;
	static hls::stream<axiWord> internalData3;

#pragma HLS stream depth=16 variable=internalData0
#pragma HLS stream depth=16 variable=internalData1
#pragma HLS stream depth=16 variable=internalData2
#pragma HLS stream depth=16 variable=internalData3

	slvsrx_lanealigner_t<0>(inData0, internalData0);
	slvsrx_lanealigner_t<1>(inData1, internalData1);
	slvsrx_lanealigner_t<2>(inData2, internalData2);
	slvsrx_lanealigner_t<3>(inData3, internalData3);

//	static hls::stream<axiWord> internalData0_1;
//	while(!internalData0.empty()) {
//		auto t = internalData0.read();
//		std::cout << "I0: " << std::hex << t.data << " " << t.last << std::endl;
//		internalData0_1.write(t);
//	}


	slvsrx_merger(internalData0, internalData1, internalData2, internalData3, outData, framestart, frameend, linevalid, curline, lastPacketSize);

}

