#**********************************************************************
# Author : Charles Wu
#**********************************************************************

#-------------------------------------------------------
# Release Candidate : Must be 0 for FORMAL Release 
#-------------------------------------------------------
#-- RC Version : 2-digit decimal
RC_VER := 0

#-------------------------------------------------------
# BUILD_VER
#-------------------------------------------------------
#-- BUILD_VER_MAJOR : 2-digit decimal
B_VER_MAJOR := 1

#-- BUILD_VER_MAJOR : 3-digit decimal
B_VER_MINOR := 1

#-------------------------------------------------------
# ISP FW Formal Release
# Format : ISP-F-%AA%B%-V%C%.%XX%.%YY%
#-------------------------------------------------------
#-- ISP Generation : 10=C1, 20=C2, 30=C3, 500=C5 ISP, 510=C5 PT, 520=C5B ISP  
FWVER_AA := 92

#-- ID of Codec : 0=FIC8180, 1=ACTi-A1
FWVER_B := 3

#-- Prime Code : 1 digit, starting from 1
FWVER_C := 1

#-- Major Code : 2 digits, starting from 0
FWVER_XX := 1

#-- Minor Code : 2 digitsl, starting from 0
FWVER_YY := 13


#-------------------------------------------------------
# ISP HW UPDATE Release
# Format : ISP-H-%AA%B%-V%C%.%XX%.%YY%
#-------------------------------------------------------
#-- ISP Generation : 10=C1, 20=C2, 30=C3, 500=C5 ISP, 510=C5 PT, 520=C5B ISP
HWVER_AA := 92

#-- ID of Codec : 0=FIC8180, 1=ACTi-A1
HWVER_B := 3

#-- Prime Code : 1 digit, starting from 1
HWVER_C := 0

#-- Major Code : 2 digits, starting from 0
HWVER_XX := 0

#-- Minor Code : 2 digitsl, starting from 0
HWVER_YY := 10

