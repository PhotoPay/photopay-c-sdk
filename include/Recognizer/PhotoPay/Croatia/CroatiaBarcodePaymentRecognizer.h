/**
 * @file CroatiaBarcodePaymentRecognizer.h
 *
 * Copyright (c) Microblink Ltd. All rights reserved.
 *
 * ANY UNAUTHORIZED USE OR SALE, DUPLICATION, OR DISTRIBUTION
 * OF THIS PROGRAM OR ANY OF ITS PARTS, IN SOURCE OR BINARY FORMS,
 * WITH OR WITHOUT MODIFICATION, WITH THE PURPOSE OF ACQUIRING
 * UNLAWFUL MATERIAL OR ANY OTHER BENEFIT IS PROHIBITED!
 * THIS PROGRAM IS PROTECTED BY COPYRIGHT LAWS AND YOU MAY NOT
 * REVERSE ENGINEER, DECOMPILE, OR DISASSEMBLE IT.
 */

#ifndef CROATIA_BARCODE_PAYMENT_RECOGNIZER
#define CROATIA_BARCODE_PAYMENT_RECOGNIZER

#include <Recognizer/Export.h>
#include <Recognizer/RecognizerError.h>
#include <Recognizer/Types.h>
#include <Recognizer/Recognizer.h>
#include <Utils/Date.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @struct MBCroatiaCommonBarcodePaymentRecognizerSettings
 * @brief Common settings for both Croatian PDF417 and QR payment recognizer.
 */
struct MBCroatiaCommonBarcodePaymentRecognizerSettings
{
    /**
     * The rate at which the conversion from EUR to HRK will be made.
     * By default, this is set to '7.5345'.
     */
    float conversionRate;
};

/**
 * @brief Typedef for the MBCroatiaCommonBarcodePaymentRecognizerSettings structure.
 */
MB_EXPORTED_TYPE typedef struct MBCroatiaCommonBarcodePaymentRecognizerSettings MBCroatiaCommonBarcodePaymentRecognizerSettings;

struct MBCroatiaPdf417PaymentRecognizerSettings;

/**
 * @brief Typedef for the MBCroatiaPdf417PaymentRecognizerSettings structure.
 */
MB_EXPORTED_TYPE typedef struct MBCroatiaPdf417PaymentRecognizerSettings MBCroatiaPdf417PaymentRecognizerSettings;

/**
 * @memberof MBCroatiaPdf417PaymentRecognizerSettings
 * @brief Populate MBCroatiaPdf417PaymentRecognizerSettings structure with default values.
 * @return Nothing
 */
MB_API void MB_CALL croatiaPdf417PaymentRecognizerSettingsInit( MBCroatiaPdf417PaymentRecognizerSettings * );

/**
 * @struct MBCroatiaPdf417PaymentRecognizerSettings
 * @brief Settings for configuring MBCroatiaPdf417PaymentRecognizer
 */
struct MBCroatiaPdf417PaymentRecognizerSettings
{
    /**
     * Common settings shared between both PDF and QR payment recognizers.
     */
    MBCroatiaCommonBarcodePaymentRecognizerSettings common;

    /**
     * Uncertain decoding enables scanning of non-standard elements, but there is no guarantee that all data will be read.
     * For Pdf417 barcode is used when multiple rows are missing (e.g. not whole barcode is printed).
     * By default, this is set to true.
     */
    MBBool uncertainDecoding;

#ifdef __cplusplus
    /** Default constructor for c++ */
    MBCroatiaPdf417PaymentRecognizerSettings()
    {
        croatiaPdf417PaymentRecognizerSettingsInit( this );
    }
#endif
};

struct MBCroatiaQrPaymentRecognizerSettings;

/**
 * @brief Typedef for the MBCroatiaQrPaymentRecognizerSettings structure
 */
MB_EXPORTED_TYPE typedef struct MBCroatiaQrPaymentRecognizerSettings MBCroatiaQrPaymentRecognizerSettings;

/**
 * @memberof MBCroatiaQrPaymentRecognizerSettings
 * @brief Populate MBCroatiaQrPaymentRecognizerSettings structure with default values.
 * @return Nothing
 */
MB_API void MB_CALL croatiaQrPaymentRecognizerSettingsInit( MBCroatiaQrPaymentRecognizerSettings * );

/**
 * @struct MBCroatiaQrPaymentRecognizerSettings
 * @brief Settings for configuring MBCroatiaQrPaymentRecognizer
 */
struct MBCroatiaQrPaymentRecognizerSettings
{
    /**
     * Common settings shared between both PDF and QR payment recognizers.
     */
    MBCroatiaCommonBarcodePaymentRecognizerSettings common;

    /**
     * Defines if the recognizer should go slower through scan.
     *  If slowerThoroughScan is enabled, then scanning will be slower, but more thorough, thus giving higher possibility of successful scan.
     *  By default, slowerThoroughScan is enabled.
     */
    MBBool slowerThoroughScan;

#ifdef __cplusplus
    /** Default constructor for c++ */
    MBCroatiaQrPaymentRecognizerSettings()
    {
        croatiaQrPaymentRecognizerSettingsInit( this );
    }
#endif
};

/**
 * @brief Type of scanned payment barcode
 */
enum MBCroatiaBarcodePaymentSlipIdType
{
    /** Scanned barcode was of HUB1 type */
    MB_CROATIA_PAYMENT_BARCODE_TYPE_HUB1 = 0,

    /** Scanned barcode was of HUB3 type */
    MB_CROATIA_PAYMENT_BARCODE_TYPE_HUB3
};

/**
 * @brief Typedef for the MBCroatiaBarcodePaymentSlipIdType enum
 */
MB_EXPORTED_TYPE typedef enum MBCroatiaBarcodePaymentSlipIdType MBCroatiaBarcodePaymentSlipIdType;

/**
 * @struct MBCroatiaBarcodePaymentRecognizerResult
 * @brief A result that can be obtained from either MBCroatiaQrPaymentRecognizer or MBCroatiaPdf417PaymentRecognizer
 */
struct MBCroatiaBarcodePaymentRecognizerResult
{
    /** Base result common to all recognizer results. */
    MBBaseRecognizerResult baseResult;

    /** The scanned amount in smallest HRK monetary unit, i.e. lipa, 0 if nothing was scanned. */
    int amountHrk;

    /** The scanned amount in smallest EUR monetary unit, i.e. cent, 0 if nothing was scanned. */
    int amountEur;

    /** The information if the conversion from HRK to EUR is performed or not */
    MBBool conversionToEurPerformed;

    /** The name of the payer */
    char const * payerName;

    /** The address of the payer */
    char const * payerAddress;

    /** The detailed address of the payer; available only for HUB3 slips */
    char const * payerDetailedAddress;

    /** The name of the receiving side */
    char const * recipientName;

    /** The address of the payment receiver */
    char const * recipientAddress;

    /** The detailed address of the payment receiver; available only for HUB3 slips */
    char const * recipientDetailedAddress;

    /** The account number to which the payment goes */
    char const * accountNumber;

    /** The bank code of the receiver bank */
    char const * bankCode;

    /** The International bank account number of the account to which the payment goes */
    char const * iban;

    /** The reference model of the payment */
    char const * referenceModel;

    /** The reference of the payment */
    char const * reference;

    /** The purpose code of the payment */
    char const * purposeCode;

    /** The description of the payment */
    char const * paymentDescription;

    /** The description code of the payment */
    char const * paymentDescriptionCode;

    /** The due date for payment; available only for HUB3 slips */
    MBDate  dueDate;

    /** The additional data available at the end of HUB3 QR and PDF417 barcode */
    char const * optionalData;

    /** Slip ID (e.g MB_CROATIA_PAYMENT_BARCODE_TYPE_HUB3) */
    MBCroatiaBarcodePaymentSlipIdType slipId;

    /** Indicates whether or not scanned barcode was uncertain */
    MBBool uncertain;
};

/**
 * @brief Typedef for the MBCroatiaBarcodePaymentRecognizerResult structure.
 */
MB_EXPORTED_TYPE typedef struct MBCroatiaBarcodePaymentRecognizerResult MBCroatiaBarcodePaymentRecognizerResult;

/**
 * @struct MBCroatiaPdf417PaymentRecognizer
 * @brief Recognizer which can scan Croatian payment PDF417 2D barcode.
 */
struct MBCroatiaPdf417PaymentRecognizer;

/**
 * @brief Typedef for the MBCroatiaPdf417PaymentRecognizer structure
 */
MB_EXPORTED_TYPE typedef struct MBCroatiaPdf417PaymentRecognizer MBCroatiaPdf417PaymentRecognizer;

/**
 * @brief Typedef for the MBCroatiaBarcodePaymentRecognizerResult structure - convenience usage for PDF417 recognizer.
 */
MB_EXPORTED_TYPE typedef struct MBCroatiaBarcodePaymentRecognizerResult MBCroatiaPdf417PaymentRecognizerResult;

/**
 * @memberof MBCroatiaPdf417PaymentRecognizer
 * @brief Allocates and initializes new MBCroatiaPdf417PaymentRecognizer object.
 * @param croatiaPdf417PaymentRecognizer Pointer to pointer referencing the created MBCroatiaPdf417PaymentRecognizer object.
 * @param croatiaPdf417PaymentRecognizerSettings Settings that will be used for creating of the MBCroatiaPdf417PaymentRecognizer object.
 * @return status of the operation. The operation may fail (i.e. if license key is not set or does not allow
 *         usage of the requested recognizer), so please check the returned status for possible errors.
 */
MB_API MBRecognizerErrorStatus MB_CALL croatiaPdf417PaymentRecognizerCreate( MBCroatiaPdf417PaymentRecognizer ** croatiaPdf417PaymentRecognizer, MBCroatiaPdf417PaymentRecognizerSettings const * croatiaPdf417PaymentRecognizerSettings );

/**
 * @memberof MBCroatiaPdf417PaymentRecognizer
 * @brief Updates the MBCroatiaPdf417PaymentRecognizer with the new settings.
 * Note that updating the recognizer while it is being in use by MBRecognizerRunner will fail.
 * @param croatiaPdf417PaymentRecognizer MBCroatiaPdf417PaymentRecognizer that should be updated with new settings.
 * @param croatiaPdf417PaymentRecognizerSettings Settings that will be used for updating the MBCroatiaPdf417PaymentRecognizer object.
 * @return status of the operation. The operation may fail (i.e. if recognizer is in use by the MBRecognizerRunner),
 *         so please check the returned status for possible errors.
 */
MB_API MBRecognizerErrorStatus MB_CALL croatiaPdf417PaymentRecognizerUpdate( MBCroatiaPdf417PaymentRecognizer * croatiaPdf417PaymentRecognizer, MBCroatiaPdf417PaymentRecognizerSettings const * croatiaPdf417PaymentRecognizerSettings );

/**
 * @memberof MBCroatiaPdf417PaymentRecognizer
 * @brief Obtains the result from the given MBCroatiaPdf417PaymentRecognizer object.
 * @param result MBCroatiaPdf417PaymentRecognizerResult structure that will be filled with the recognized data.
 * Note that all pointers in structure will remain valid until given recognizer is
 * destroyed with croatiaPdf417PaymentRecognizerDelete function or is used for performing the new recognition.
 * @param croatiaPdf417PaymentRecognizer MBCroatiaPdf417PaymentRecognizer from which result should be obtained.
 * @return status of the operation.
 */
MB_API MBRecognizerErrorStatus MB_CALL croatiaPdf417PaymentRecognizerResult( MBCroatiaPdf417PaymentRecognizerResult * result, MBCroatiaPdf417PaymentRecognizer const * croatiaPdf417PaymentRecognizer );

/**
 * @memberof MBCroatiaPdf417PaymentRecognizer
 * @brief Destroys the given MBCroatiaPdf417PaymentRecognizer.
 * @param croatiaPdf417PaymentRecognizer Pointer to pointer to MBCroatiaPdf417PaymentRecognizer structure that needs to be destroyed.
 * After destruction, the pointer to MBCroatiaPdf417PaymentRecognizer structure will be set to NULL.
 * @return status of the operation
 */
MB_API MBRecognizerErrorStatus MB_CALL croatiaPdf417PaymentRecognizerDelete( MBCroatiaPdf417PaymentRecognizer ** croatiaPdf417PaymentRecognizer );

/**
 * @struct MBCroatiaQrPaymentRecognizer
 * @brief Recognizer which can scan Croatian payment PDF417 2D barcode.
 */
struct MBCroatiaQrPaymentRecognizer;

/**
 * @brief Typedef for the MBCroatiaQrPaymentRecognizer structure
 */
MB_EXPORTED_TYPE typedef struct MBCroatiaQrPaymentRecognizer MBCroatiaQrPaymentRecognizer;

/**
 * @brief Typedef for the MBCroatiaBarcodePaymentRecognizerResult structure - convenience usage for QR Code recognizer.
 */
MB_EXPORTED_TYPE typedef struct MBCroatiaBarcodePaymentRecognizerResult MBCroatiaQrPaymentRecognizerResult;

/**
 * @memberof MBCroatiaQrPaymentRecognizer
 * @brief Allocates and initializes new MBCroatiaQrPaymentRecognizer object.
 * @param croatiaPdf417PaymentRecognizer Pointer to pointer referencing the created MBCroatiaQrPaymentRecognizer object.
 * @param croatiaQrPaymentRecognizerSettings Settings that will be used for creating of the MBCroatiaQrPaymentRecognizer object.
 * @return status of the operation. The operation may fail (i.e. if license key is not set or does not allow
 *         usage of the requested recognizer), so please check the returned status for possible errors.
 */
MB_API MBRecognizerErrorStatus MB_CALL croatiaQrPaymentRecognizerCreate( MBCroatiaQrPaymentRecognizer ** croatiaPdf417PaymentRecognizer, MBCroatiaQrPaymentRecognizerSettings const * croatiaQrPaymentRecognizerSettings );

/**
 * @memberof MBCroatiaQrPaymentRecognizer
 * @brief Updates the MBCroatiaQrPaymentRecognizer with the new settings.
 * Note that updating the recognizer while it is being in use by MBRecognizerRunner will fail.
 * @param croatiaQrPaymentRecognizer MBCroatiaQrPaymentRecognizer that should be updated with new settings.
 * @param croatiaQrPaymentRecognizerSettings Settings that will be used for updating the MBCroatiaQrPaymentRecognizer object.
 * @return status of the operation. The operation may fail (i.e. if recognizer is in use by the MBRecognizerRunner),
 *         so please check the returned status for possible errors.
 */
MB_API MBRecognizerErrorStatus MB_CALL croatiaQrPaymentRecognizerUpdate( MBCroatiaQrPaymentRecognizer * croatiaQrPaymentRecognizer, MBCroatiaQrPaymentRecognizerSettings const * croatiaQrPaymentRecognizerSettings );

/**
 * @memberof MBCroatiaQrPaymentRecognizer
 * @brief Obtains the result from the given MBCroatiaQrPaymentRecognizer object.
 * @param result MBCroatiaQrPaymentRecognizerResult structure that will be filled with the recognized data.
 * Note that all pointers in structure will remain valid until given recognizer is
 * destroyed with croatiaQrPaymentRecognizerDelete function or is used for performing the new recognition.
 * @param croatiaQrPaymentRecognizer MBCroatiaQrPaymentRecognizer from which result should be obtained.
 * @return status of the operation.
 */
MB_API MBRecognizerErrorStatus MB_CALL croatiaQrPaymentRecognizerResult( MBCroatiaQrPaymentRecognizerResult * result, MBCroatiaQrPaymentRecognizer const * croatiaQrPaymentRecognizer );

/**
 * @memberof MBCroatiaQrPaymentRecognizer
 * @brief Destroys the given MBCroatiaQrPaymentRecognizer.
 * @param croatiaQrPaymentRecognizer Pointer to pointer to MBCroatiaQrPaymentRecognizer structure that needs to be destroyed.
 * After destruction, the pointer to MBCroatiaQrPaymentRecognizer structure will be set to NULL.
 * @return status of the operation
 */
MB_API MBRecognizerErrorStatus MB_CALL croatiaQrPaymentRecognizerDelete( MBCroatiaQrPaymentRecognizer ** croatiaQrPaymentRecognizer );

#ifdef __cplusplus
}
#endif

#endif
