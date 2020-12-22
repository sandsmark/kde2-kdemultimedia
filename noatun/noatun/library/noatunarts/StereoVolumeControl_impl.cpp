
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <artsflow.h>
#include <stdsynthmodule.h>
#include <flowsystem.h>
#include "noatunarts.h"

using namespace Arts;

namespace Noatun
{

class StereoVolumeControl_impl : virtual public StereoVolumeControl_skel,
                                 virtual public StdSynthModule
{
	float mPercent;
public:
	StereoVolumeControl_impl() : mPercent(1.0)
	{ }

	/*attribute float scaleFactor;*/
	void percent(float p) { mPercent=p; }
	float percent() { return mPercent; }

	void calculateBlock(unsigned long samples)
	{
		float *left=inleft;
		float *right=inright;
		float *oleft=outleft;
		float *oright=outright;

		register float p=mPercent;

		float *end=left+samples;

		while (left<end)
		{
			*oleft=*left * p;
			*oright=*right * p;

			++left;
			++right;
			++oleft;
			++oright;
		}
	}
};
	
class StereoVolumeControlSSE_impl : virtual public Noatun::StereoVolumeControlSSE_skel,
                                    virtual public StdSynthModule
{
	float mPercent;
public:
	StereoVolumeControlSSE_impl() : mPercent(1.0)
	{ }

	/*attribute float scaleFactor;*/
	void percent(float p) { mPercent=p; }
	float percent() { return mPercent; }

	void calculateBlock(unsigned long samples)
	{
#ifdef HAVE_X86_SSE
		float *left=inleft;
		float *right=inright;
		float *oleft=outleft;
		float *oright=outright;

		// need to copy the data members to locals to get enough
		// spare registers (malte)
		
		long p = (long)(mPercent*100.0);
		__asm__ __volatile__(
			"pushl $100                       \n"
			"fildl (%%esp)                    \n"
			"fildl %5                         \n"
			"fdivp                            \n" // percent / 100.0
			"fstps (%%esp)                    \n"
			"movss (%%esp), %%xmm1            \n"
			"shufps $0x00, %%xmm1, %%xmm1     \n" // percentage in all of xmm1
			"addl $4, %%esp                   \n"
			"pushl %4                         \n" // save sample count
			"shrl $2, %4                      \n"
			"jz .l2                           \n" // samples < 4
			"xorl %%ecx, %%ecx                \n"

			".l1:                             \n"
			// left
			"movups (%0, %%ecx, 8), %%xmm0    \n"
			"mulps %%xmm1, %%xmm0             \n"
			"movl %2, %%eax                   \n"
			"movups %%xmm0, (%%eax, %%ecx, 8) \n"
			// right
			"movups (%1, %%ecx, 8), %%xmm0    \n"
			"mulps %%xmm1, %%xmm0             \n"
			"movl %3, %%eax                   \n"
			"movups %%xmm0, (%%eax, %%ecx, 8) \n"
		
			"incl %%ecx                       \n"
			"incl %%ecx                       \n"
			"decl %4                          \n"
			"jnz .l1                          \n"
			".l2:                             \n"
			"popl %4                          \n" // restore sample count
			"andl $3, %4                      \n"
			"jz .l4                           \n"

			// calculate remaining samples for samples % 4 != 0
			"shll $1, %%ecx                   \n"
			".l3:                             \n"
			"movss (%0, %%ecx, 4), %%xmm0     \n" // load left
			"movss (%1, %%ecx, 4), %%xmm2     \n" // load right
			"shufps $0x00, %%xmm2, %%xmm0     \n" // both channels in xmm0
			"mulps %%xmm1, %%xmm0             \n"
			"movl %2, %%eax                   \n"
			"movss %%xmm0, (%%eax, %%ecx, 4)  \n" // store left
			"shufps $0x02, %%xmm0, %%xmm0     \n"
			"movl %3, %%eax                   \n"
			"movss %%xmm0, (%%eax, %%ecx, 4)  \n" // store right
			"incl %%ecx                       \n"	
			"decl %4                          \n"
			"jnz .l3                          \n"
			
			".l4:                             \n"
			"emms                             \n"
			:
			: "r" (left),     // %0
			  "r" (right),    // %1
			  "m" (oleft),    // %2
			  "m" (oright),   // %3
			  "r" (samples),  // %4
			  "m" (p)         // %5
			: "eax", "ecx"
		);
#endif
	}
};

REGISTER_IMPLEMENTATION(StereoVolumeControlSSE_impl);
REGISTER_IMPLEMENTATION(StereoVolumeControl_impl);

};

