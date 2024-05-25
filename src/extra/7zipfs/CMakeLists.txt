

# Check makefile in srcdir/7zip-main/CPP/7zip/Archive/7z/makefile
set(7zip_src
        7zip-main/CPP/7zip/Compress/Bcj2Coder.cpp
        7zip-main/CPP/7zip/Compress/Bcj2Register.cpp
        7zip-main/CPP/7zip/Compress/BcjCoder.cpp
        7zip-main/CPP/7zip/Compress/BcjRegister.cpp
        7zip-main/CPP/7zip/Compress/BitlDecoder.cpp
        7zip-main/CPP/7zip/Compress/BranchMisc.cpp
        7zip-main/CPP/7zip/Compress/BranchRegister.cpp
        7zip-main/CPP/7zip/Compress/ByteSwap.cpp
        7zip-main/CPP/7zip/Compress/BZip2Crc.cpp
        7zip-main/CPP/7zip/Compress/BZip2Decoder.cpp
        7zip-main/CPP/7zip/Compress/BZip2Encoder.cpp
        7zip-main/CPP/7zip/Compress/BZip2Register.cpp
        7zip-main/CPP/7zip/Compress/CodecExports.cpp
        7zip-main/CPP/7zip/Compress/CopyCoder.cpp
        7zip-main/CPP/7zip/Compress/CopyRegister.cpp
        7zip-main/CPP/7zip/Compress/Deflate64Register.cpp
        7zip-main/CPP/7zip/Compress/DeflateDecoder.cpp
        7zip-main/CPP/7zip/Compress/DeflateEncoder.cpp
        7zip-main/CPP/7zip/Compress/DeflateRegister.cpp
        7zip-main/CPP/7zip/Compress/DeltaFilter.cpp
        7zip-main/CPP/7zip/Compress/ImplodeDecoder.cpp
        7zip-main/CPP/7zip/Compress/ImplodeHuffmanDecoder.cpp
        7zip-main/CPP/7zip/Compress/LzfseDecoder.cpp
        7zip-main/CPP/7zip/Compress/LzhDecoder.cpp
        7zip-main/CPP/7zip/Compress/Lzma2Decoder.cpp
        7zip-main/CPP/7zip/Compress/Lzma2Encoder.cpp
        7zip-main/CPP/7zip/Compress/Lzma2Register.cpp
        7zip-main/CPP/7zip/Compress/LzmaDecoder.cpp
        7zip-main/CPP/7zip/Compress/LzmaEncoder.cpp
        7zip-main/CPP/7zip/Compress/LzmaRegister.cpp
        7zip-main/CPP/7zip/Compress/LzmsDecoder.cpp
        7zip-main/CPP/7zip/Compress/LzOutWindow.cpp
        7zip-main/CPP/7zip/Compress/LzxDecoder.cpp
        7zip-main/CPP/7zip/Compress/PpmdDecoder.cpp
        7zip-main/CPP/7zip/Compress/PpmdEncoder.cpp
        7zip-main/CPP/7zip/Compress/PpmdRegister.cpp
        7zip-main/CPP/7zip/Compress/PpmdZip.cpp
        7zip-main/CPP/7zip/Compress/QuantumDecoder.cpp
        7zip-main/CPP/7zip/Compress/Rar1Decoder.cpp
        7zip-main/CPP/7zip/Compress/Rar2Decoder.cpp
        7zip-main/CPP/7zip/Compress/Rar3Decoder.cpp
        7zip-main/CPP/7zip/Compress/Rar3Vm.cpp
        7zip-main/CPP/7zip/Compress/Rar5Decoder.cpp
        7zip-main/CPP/7zip/Compress/RarCodecsRegister.cpp
        7zip-main/CPP/7zip/Compress/ShrinkDecoder.cpp
        7zip-main/CPP/7zip/Compress/XpressDecoder.cpp
        7zip-main/CPP/7zip/Compress/XzDecoder.cpp
        7zip-main/CPP/7zip/Compress/XzEncoder.cpp
        7zip-main/CPP/7zip/Compress/ZDecoder.cpp
        7zip-main/CPP/7zip/Compress/ZlibDecoder.cpp
        7zip-main/CPP/7zip/Compress/ZlibEncoder.cpp

        7zip-main/CPP/7zip/Crypto/7zAes.cpp
        7zip-main/CPP/7zip/Crypto/7zAesRegister.cpp
        7zip-main/CPP/7zip/Crypto/HmacSha1.cpp
        7zip-main/CPP/7zip/Crypto/HmacSha256.cpp
        7zip-main/CPP/7zip/Crypto/MyAes.cpp
        7zip-main/CPP/7zip/Crypto/MyAesReg.cpp
        7zip-main/CPP/7zip/Crypto/Pbkdf2HmacSha1.cpp
        7zip-main/CPP/7zip/Crypto/RandGen.cpp
        7zip-main/CPP/7zip/Crypto/Rar20Crypto.cpp
        7zip-main/CPP/7zip/Crypto/Rar5Aes.cpp
        7zip-main/CPP/7zip/Crypto/RarAes.cpp
        7zip-main/CPP/7zip/Crypto/WzAes.cpp
        7zip-main/CPP/7zip/Crypto/ZipCrypto.cpp
        7zip-main/CPP/7zip/Crypto/ZipStrong.cpp

        7zip-main/C/7zBuf2.c
        7zip-main/C/7zStream.c
        7zip-main/C/Alloc.c
        7zip-main/C/Bcj2.c
        7zip-main/C/Bcj2Enc.c
        7zip-main/C/Blake2s.c
        7zip-main/C/Bra.c
        7zip-main/C/Bra86.c
        7zip-main/C/BraIA64.c
        7zip-main/C/BwtSort.c
        7zip-main/C/CpuArch.c
        7zip-main/C/Delta.c
        7zip-main/C/HuffEnc.c
        7zip-main/C/LzFind.c
        7zip-main/C/LzFindMt.c
        7zip-main/C/Lzma2Dec.c
        7zip-main/C/Lzma2DecMt.c
        7zip-main/C/Lzma2Enc.c
        7zip-main/C/LzmaEnc.c
        7zip-main/C/LzmaLib.c
        7zip-main/C/MtCoder.c
        7zip-main/C/MtDec.c
        7zip-main/C/Ppmd7.c
        7zip-main/C/Ppmd7aDec.c
        7zip-main/C/Ppmd7Dec.c
        7zip-main/C/Ppmd7Enc.c
        7zip-main/C/Ppmd8.c
        7zip-main/C/Ppmd8Dec.c
        7zip-main/C/Ppmd8Enc.c
        7zip-main/C/Sha1.c
        7zip-main/C/Sha256.c
        7zip-main/C/Sort.c
        7zip-main/C/SwapBytes.c
        7zip-main/C/Threads.c
        7zip-main/C/Xz.c
        7zip-main/C/XzDec.c
        7zip-main/C/XzEnc.c
        7zip-main/C/XzIn.c
        7zip-main/C/Aes.c
        7zip-main/C/7zCrc.c
        7zip-main/C/XzCrc64.c

        7zip-main/CPP/Common/CRC.cpp
        7zip-main/CPP/Common/CrcReg.cpp
        7zip-main/CPP/Common/DynLimBuf.cpp
        7zip-main/CPP/Common/IntToString.cpp
        7zip-main/CPP/Common/LzFindPrepare.cpp
        7zip-main/CPP/Common/MyMap.cpp
        7zip-main/CPP/Common/MyString.cpp
        7zip-main/CPP/Common/MyVector.cpp
        7zip-main/CPP/Common/MyWindows.cpp
        7zip-main/CPP/Common/MyXml.cpp
        7zip-main/CPP/Common/NewHandler.cpp
        7zip-main/CPP/Common/Sha1Prepare.cpp
        7zip-main/CPP/Common/Sha1Reg.cpp
        7zip-main/CPP/Common/Sha256Prepare.cpp
        7zip-main/CPP/Common/Sha256Reg.cpp
        7zip-main/CPP/Common/StringConvert.cpp
        7zip-main/CPP/Common/StringToInt.cpp
        7zip-main/CPP/Common/UTFConvert.cpp
        7zip-main/CPP/Common/Wildcard.cpp
        7zip-main/CPP/Common/XzCrc64Init.cpp
        7zip-main/CPP/Common/XzCrc64Reg.cpp

        7zip-main/CPP/Windows/FileDir.cpp
        7zip-main/CPP/Windows/FileFind.cpp
        7zip-main/CPP/Windows/FileIO.cpp
        7zip-main/CPP/Windows/FileName.cpp
        7zip-main/CPP/Windows/PropVariant.cpp
        7zip-main/CPP/Windows/PropVariantUtils.cpp
        7zip-main/CPP/Windows/Synchronization.cpp
        7zip-main/CPP/Windows/System.cpp
        7zip-main/CPP/Windows/TimeUtils.cpp

        7zip-main/CPP/7zip/Common/CreateCoder.cpp
        7zip-main/CPP/7zip/Common/CWrappers.cpp
        7zip-main/CPP/7zip/Common/InBuffer.cpp
        7zip-main/CPP/7zip/Common/InOutTempBuffer.cpp
        7zip-main/CPP/7zip/Common/FilterCoder.cpp
        7zip-main/CPP/7zip/Common/LimitedStreams.cpp
        7zip-main/CPP/7zip/Common/LockedStream.cpp
        7zip-main/CPP/7zip/Common/MemBlocks.cpp
        7zip-main/CPP/7zip/Common/MethodId.cpp
        7zip-main/CPP/7zip/Common/MethodProps.cpp
        7zip-main/CPP/7zip/Common/OffsetStream.cpp
        7zip-main/CPP/7zip/Common/OutBuffer.cpp
        7zip-main/CPP/7zip/Common/OutMemStream.cpp
        7zip-main/CPP/7zip/Common/ProgressMt.cpp
        7zip-main/CPP/7zip/Common/ProgressUtils.cpp
        7zip-main/CPP/7zip/Common/PropId.cpp
        7zip-main/CPP/7zip/Common/StreamBinder.cpp
        7zip-main/CPP/7zip/Common/StreamObjects.cpp
        7zip-main/CPP/7zip/Common/StreamUtils.cpp
        7zip-main/CPP/7zip/Common/UniqBlocks.cpp
        7zip-main/CPP/7zip/Common/VirtThread.cpp

        7zip-main/CPP/7zip/Archive/ApmHandler.cpp
        7zip-main/CPP/7zip/Archive/ArHandler.cpp
        7zip-main/CPP/7zip/Archive/ArjHandler.cpp
        7zip-main/CPP/7zip/Archive/Base64Handler.cpp
        7zip-main/CPP/7zip/Archive/Bz2Handler.cpp
        7zip-main/CPP/7zip/Archive/ComHandler.cpp
        7zip-main/CPP/7zip/Archive/CpioHandler.cpp
        7zip-main/CPP/7zip/Archive/CramfsHandler.cpp
        7zip-main/CPP/7zip/Archive/DeflateProps.cpp
        7zip-main/CPP/7zip/Archive/DmgHandler.cpp
        7zip-main/CPP/7zip/Archive/ElfHandler.cpp
        7zip-main/CPP/7zip/Archive/ExtHandler.cpp
        7zip-main/CPP/7zip/Archive/FatHandler.cpp
        7zip-main/CPP/7zip/Archive/FlvHandler.cpp
        7zip-main/CPP/7zip/Archive/GzHandler.cpp
        7zip-main/CPP/7zip/Archive/GptHandler.cpp
        7zip-main/CPP/7zip/Archive/HandlerCont.cpp
        7zip-main/CPP/7zip/Archive/HfsHandler.cpp
        7zip-main/CPP/7zip/Archive/IhexHandler.cpp
        7zip-main/CPP/7zip/Archive/LzhHandler.cpp
        7zip-main/CPP/7zip/Archive/LzmaHandler.cpp
        7zip-main/CPP/7zip/Archive/MachoHandler.cpp
        7zip-main/CPP/7zip/Archive/MbrHandler.cpp
        7zip-main/CPP/7zip/Archive/MslzHandler.cpp
        7zip-main/CPP/7zip/Archive/MubHandler.cpp
        7zip-main/CPP/7zip/Archive/NtfsHandler.cpp
        7zip-main/CPP/7zip/Archive/PeHandler.cpp
        7zip-main/CPP/7zip/Archive/PpmdHandler.cpp
        7zip-main/CPP/7zip/Archive/QcowHandler.cpp
        7zip-main/CPP/7zip/Archive/RpmHandler.cpp
        7zip-main/CPP/7zip/Archive/SplitHandler.cpp
        7zip-main/CPP/7zip/Archive/SquashfsHandler.cpp
        7zip-main/CPP/7zip/Archive/SwfHandler.cpp
        7zip-main/CPP/7zip/Archive/UefiHandler.cpp
        7zip-main/CPP/7zip/Archive/VdiHandler.cpp
        7zip-main/CPP/7zip/Archive/VhdHandler.cpp
        7zip-main/CPP/7zip/Archive/VmdkHandler.cpp
        7zip-main/CPP/7zip/Archive/XarHandler.cpp
        7zip-main/CPP/7zip/Archive/XzHandler.cpp
        7zip-main/CPP/7zip/Archive/ZHandler.cpp
        7zip-main/CPP/7zip/Archive/ArchiveExports.cpp
        7zip-main/CPP/7zip/Archive/DllExports2.cpp

        7zip-main/CPP/7zip/Archive/Common/CoderMixer2.cpp
        7zip-main/CPP/7zip/Archive/Common/DummyOutStream.cpp
        7zip-main/CPP/7zip/Archive/Common/FindSignature.cpp
        7zip-main/CPP/7zip/Archive/Common/InStreamWithCRC.cpp
        7zip-main/CPP/7zip/Archive/Common/ItemNameUtils.cpp
        7zip-main/CPP/7zip/Archive/Common/MultiStream.cpp
        7zip-main/CPP/7zip/Archive/Common/OutStreamWithCRC.cpp
        7zip-main/CPP/7zip/Archive/Common/OutStreamWithSha1.cpp
        7zip-main/CPP/7zip/Archive/Common/HandlerOut.cpp
        7zip-main/CPP/7zip/Archive/Common/ParseProperties.cpp

        7zip-main/CPP/7zip/Archive/7z/7zCompressionMode.cpp
        7zip-main/CPP/7zip/Archive/7z/7zDecode.cpp
        7zip-main/CPP/7zip/Archive/7z/7zEncode.cpp
        7zip-main/CPP/7zip/Archive/7z/7zExtract.cpp
        7zip-main/CPP/7zip/Archive/7z/7zFolderInStream.cpp
        7zip-main/CPP/7zip/Archive/7z/7zHandler.cpp
        7zip-main/CPP/7zip/Archive/7z/7zHandlerOut.cpp
        7zip-main/CPP/7zip/Archive/7z/7zHeader.cpp
        7zip-main/CPP/7zip/Archive/7z/7zIn.cpp
        7zip-main/CPP/7zip/Archive/7z/7zOut.cpp
        7zip-main/CPP/7zip/Archive/7z/7zProperties.cpp
        7zip-main/CPP/7zip/Archive/7z/7zSpecStream.cpp
        7zip-main/CPP/7zip/Archive/7z/7zUpdate.cpp
        7zip-main/CPP/7zip/Archive/7z/7zRegister.cpp

        7zip-main/CPP/7zip/Archive/Cab/CabBlockInStream.cpp
        7zip-main/CPP/7zip/Archive/Cab/CabHandler.cpp
        7zip-main/CPP/7zip/Archive/Cab/CabHeader.cpp
        7zip-main/CPP/7zip/Archive/Cab/CabIn.cpp
        7zip-main/CPP/7zip/Archive/Cab/CabRegister.cpp

        7zip-main/CPP/7zip/Archive/Chm/ChmHandler.cpp
        7zip-main/CPP/7zip/Archive/Chm/ChmIn.cpp

        7zip-main/CPP/7zip/Archive/Iso/IsoHandler.cpp
        7zip-main/CPP/7zip/Archive/Iso/IsoHeader.cpp
        7zip-main/CPP/7zip/Archive/Iso/IsoIn.cpp
        7zip-main/CPP/7zip/Archive/Iso/IsoRegister.cpp

        7zip-main/CPP/7zip/Archive/Nsis/NsisDecode.cpp
        7zip-main/CPP/7zip/Archive/Nsis/NsisHandler.cpp
        7zip-main/CPP/7zip/Archive/Nsis/NsisIn.cpp
        7zip-main/CPP/7zip/Archive/Nsis/NsisRegister.cpp

        7zip-main/CPP/7zip/Archive/Rar/RarHandler.cpp
        7zip-main/CPP/7zip/Archive/Rar/Rar5Handler.cpp

        7zip-main/CPP/7zip/Archive/Tar/TarHandler.cpp
        7zip-main/CPP/7zip/Archive/Tar/TarHandlerOut.cpp
        7zip-main/CPP/7zip/Archive/Tar/TarHeader.cpp
        7zip-main/CPP/7zip/Archive/Tar/TarIn.cpp
        7zip-main/CPP/7zip/Archive/Tar/TarOut.cpp
        7zip-main/CPP/7zip/Archive/Tar/TarUpdate.cpp
        7zip-main/CPP/7zip/Archive/Tar/TarRegister.cpp

        7zip-main/CPP/7zip/Archive/Udf/UdfHandler.cpp
        7zip-main/CPP/7zip/Archive/Udf/UdfIn.cpp

        7zip-main/CPP/7zip/Archive/Wim/WimHandler.cpp
        7zip-main/CPP/7zip/Archive/Wim/WimHandlerOut.cpp
        7zip-main/CPP/7zip/Archive/Wim/WimIn.cpp
        7zip-main/CPP/7zip/Archive/Wim/WimRegister.cpp

        7zip-main/CPP/7zip/Archive/Zip/ZipAddCommon.cpp
        7zip-main/CPP/7zip/Archive/Zip/ZipHandler.cpp
        7zip-main/CPP/7zip/Archive/Zip/ZipHandlerOut.cpp
        7zip-main/CPP/7zip/Archive/Zip/ZipIn.cpp
        7zip-main/CPP/7zip/Archive/Zip/ZipItem.cpp
        7zip-main/CPP/7zip/Archive/Zip/ZipOut.cpp
        7zip-main/CPP/7zip/Archive/Zip/ZipUpdate.cpp
        7zip-main/CPP/7zip/Archive/Zip/ZipRegister.cpp


        7zip-main/C/7zCrcOpt.c
        7zip-main/C/AesOpt.c
        7zip-main/C/LzFindOpt.c
        7zip-main/C/LzmaDec.c
        7zip-main/C/Sha1Opt.c
        7zip-main/C/Sha256Opt.c
        7zip-main/C/XzCrc64Opt.c
)

get_source_file_property(defs 7zip-main/C/LzmaDec.c COMPILE_DEFINITIONS)
list(APPEND defs "_LZMA_DEC_OPT")
set_source_files_properties(7zip-main/C/LzmaDec.c PROPERTIES COMPILE_DEFINITIONS "${defs}")

set (7zip_wrapper_src
#        7zip-wrapper/com.cc
#        7zip-wrapper/com.hh
#        7zip-wrapper/fake_windows.hh
#        7zip-wrapper/codecs.cc
#        7zip-wrapper/codecs.hh
#        7zip-wrapper/format_info.cc
#        7zip-wrapper/format_info.hh
#        7zip-wrapper/objects.hh
#        7zip-wrapper/objects.cc

    7zip-cpp/HelperFuncs.h
7zip-cpp/OSFunctions.h
7zip-cpp/7zipLibrary.cpp
7zip-cpp/7ZipCompressCodecsInfo.h
7zip-cpp/7ZipCompressCodecsInfo.cpp
7zip-cpp/7ZipDllHandler.h
7zip-cpp/7ZipArchive.cpp
7zip-cpp/7ZipArchiveItem.cpp
7zip-cpp/7ZipObjectPtrArray.cpp
7zip-cpp/OSFunctions_OS2.cpp
7zip-cpp/7ZipDllHandler.cpp
7zip-cpp/HelperFuncs.cpp
7zip-cpp/7ZipCodecInfo.h
7zip-cpp/OSFunctions_UnixLike.cpp
7zip-cpp/lib7zip.h
7zip-cpp/7ZipArchiveOpenCallback.cpp
7zip-cpp/OSFunctions_Win32.h
7zip-cpp/7ZipFormatInfo.cpp
7zip-cpp/7ZipFormatInfo.h
7zip-cpp/7ZipOpenArchive.cpp
7zip-cpp/7ZipFunctions.h
7zip-cpp/7ZipCodecInfo.cpp
7zip-cpp/OSFunctions_UnixLike.h
7zip-cpp/7ZipArchiveOpenCallback.h
7zip-cpp/OSFunctions_Win32.cpp
7zip-cpp/OSFunctions_OS2.h
7zip-cpp/7ZipInStreamWrapper.cpp
7zip-cpp/7ZipInStreamWrapper.h
)

add_library(7zipfs ${7zip_src} ${7zip_wrapper_src})

target_compile_definitions(7zipfs
        PUBLIC
        -D_REENTRANT
        -D_FILE_OFFSET_BITS=64
        -D_LARGEFILE_SOURCE
        -DEXTERNAL_CODECS
        -D_7ZIP_LARGE_PAGES
        -DUNICODE
        -D_UNICODE
)

target_include_directories(7zipfs
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/7zip-main/
        ${CMAKE_CURRENT_SOURCE_DIR}/7zip-main/CPP
        ${CMAKE_CURRENT_SOURCE_DIR}/7zip-main/C
)

add_executable (test_me test_me.cc ${7zip_src} ${7zip_wrapper_src})

target_compile_definitions (test_me
        PUBLIC
        -D_REENTRANT
        -D_FILE_OFFSET_BITS=64
        -D_LARGEFILE_SOURCE
        -DEXTERNAL_CODECS
        -D_7ZIP_LARGE_PAGES
        -DUNICODE
        -D_UNICODE
)

target_include_directories (test_me
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/7zip-main/
        ${CMAKE_CURRENT_SOURCE_DIR}/7zip-main/CPP
        ${CMAKE_CURRENT_SOURCE_DIR}/7zip-main/C
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_me PRIVATE bsw)


