%module RecognizerApiWrapper
%{
/* Includes the header in the wrapper code */
#include <RecognizerApi.h>
%}

typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef unsigned long  size_t;

/* inspired by https://stackoverflow.com/a/36047554 */
%typemap(imtype) MBRecognizerPtr "System.IntPtr"
%typemap(cstype) MBRecognizerPtr "System.IntPtr"
%typemap(csin)   MBRecognizerPtr "$csinput"
%typemap(in)     MBRecognizerPtr %{ $1 = $input; %}
%typemap(out)    MBRecognizerPtr %{ $result = $1; %}
%typemap(csout, excode=SWIGEXCODE)  MBRecognizerPtr { 
    System.IntPtr cPtr = $imcall;$excode
    return cPtr;
    }
%typemap(csvarout, excode=SWIGEXCODE2) MBRecognizerPtr %{ 
    get {
        System.IntPtr cPtr = $imcall;$excode 
        return cPtr; 
   } 
%} 

/* Parse the header file to generate wrappers */
%include "Recognizer/AvailableRecognizers.h"
%include "Recognizer/Export.h"
%include "Recognizer/Licensing.h"
%include "Recognizer/PhotoPay/Croatia/CroatiaBarcodePaymentRecognizer.h"
%include "Recognizer/Recognizer.h"
%include "Recognizer/RecognizerApiUtils.h"
%include "Recognizer/RecognizerError.h"
%include "Recognizer/RecognizerImage.h"
%include "Recognizer/RecognizerRunner.h"
%include "Recognizer/Rectangle.h"
%include "Recognizer/Types.h"
%include "RecognizerApi.h"
%include "Utils/Date.h"

%include <cpointer.i>

/* add pointer functions for opaque structures that we plan to use */
%pointer_functions(MBRecognizerImage *, MBRecognizerImage)
%pointer_functions(MBCroatiaPdf417PaymentRecognizer *, MBCroatiaPdf417PaymentRecognizer)
%pointer_functions(MBCroatiaQrPaymentRecognizer *, MBCroatiaQrPaymentRecognizer)
%pointer_functions(MBRecognizerRunner *, MBRecognizerRunner)

%include <carrays.i>
%array_functions(MBRecognizerPtr, MBRecognizerArray)

/*
 * generate wrapper by invoking:
 * swig.exe -csharp -Ipath\to\distribution\root\include -namespace Microblink -outfile RecognizerWrapper.cs -c++ RecognizerApi.i
 */
