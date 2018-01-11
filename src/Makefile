#
# extensions/src/Makefile
#

TOP = ..
include $(TOP)/configure/CONFIG

#  All EPICS extension directories may be specified here.

ADT = adt
ALH = alh
EXTENSION_AR = ar
BEAMUTIL = beamUtil
BURT = burt
CAEX = caex
CAMONITOR = camonitor
CAOBJECT2 = caObject2
CASNOOPER = caSnooper2_1_2_3
CASR = casr
CATCL = caTCL
CAU = cau
CHANNELARCHIVER = ChannelArchiver
CONDITION = condition
DALIB = dalib
DBSTATICJAVA = dbStaticJava
DBVERBOSE = dbVerbose_20130124
DCT = dct
DOUBLELINKLIST = doubleLinkList
DP = dp
ifneq ($(findstring linux,$(EPICS_HOST_ARCH)),) # linux only module
#EDM = edm
endif
EZCA = ezca
EZCAIDL = ezcaIDL
EZCASCAN = EzcaScan
ifeq ($(BASE_3_14),YES)
GATEWAY = gateway2_0_4_0
endif
GDCT313 = gdct313
GNUREGEX = gnuregex0_13
HISTTOOL = histTool
IMAGECATCHER = imageCatcher
INSTALLVERSIONS = installVersions
IOCCONSOLE = iocConsole
JCA = jca
JCA2 = jca2
JDCT = Jdct
JPROBE = jprobe
KM = km
NOBCONFIG = knobconfig
ifneq ($(findstring linux,$(EPICS_HOST_ARCH)),) # linux only module
#MEDM = medm
endif
MOTIFBUTTON = motifButton
#ifeq ($(BASE_3_14),YES)
EXTENSION_MSI = msi1-7
#endif
NAMECAPTURE = namecapture
NAMESERVER = nameserver
OAGCA = oagca
OCTAVE = Octave
PARSECASW = parsecasw
POPUP = popup
PHP_EPICS = php_epics
PROBE = probe
AMPLOAD = rampload
SCOPEUTIL = scopeutil
SDDSEPICS = SDDSepics
SDDSIDL = sddsIDL
SGA = sga
STRIPTOOL = StripTool
TS = ts
VDCT = VisualDCT
VIEWIMAGE = viewImage
YCA = Yca


# The order of the following directories is important
# Dependent tools are listed last

DIRS = $(ALH)
DIRS += $(CAOBJECT2)
DIRS += $(CASNOOPER)
DIRS += $(CATCL)  # __eprintf undefined referenced in /opt/local/lib//libtk.so
DIRS += $(CAEX)
DIRS += $(CAMONITOR)
DIRS += $(DALIB)
DIRS += $(DBSTATICJAVA)
DIRS += $(DBVERBOSE)
DIRS += $(DCT)  # __eprintf undefined referenced in /opt/local/lib//libtk.so
DIRS += $(DOUBLELINKLIST)
DIRS += $(DP)
#DIRS += $(EDM)
DIRS += $(EZCA)
DIRS += $(EZCASCAN)
DIRS += $(EZCAIDL)
DIRS += $(GDCT313)  #  /usr/include/iso/math_c99.h", line 137: Error: round(double), returning double, was previously declared returning int.
DIRS += $(GNUREGEX)
DIRS += $(GATEWAY)
DIRS += $(HISTTOOL)
DIRS += $(IMAGECATCHER)
DIRS += $(INSTALLVERSIONS)
DIRS += $(IOCCONSOLE)
DIRS += $(JCA)
DIRS += $(JCA2)
DIRS += $(JDCT)
DIRS += $(JPROBE)
DIRS += $(KM)
#DIRS += $(MEDM)
DIRS += $(MOTIFBUTTON)
DIRS += $(EXTENSION_MSI)
DIRS += $(NAMECAPTURE)
DIRS += $(NAMESERVER)
DIRS += $(OCTAVE)
DIRS += $(PARSECASW)
#DIRS += $(PHP_EPICS)
DIRS += $(POPUP)
DIRS += $(PROBE)
DIRS += $(SGA)
DIRS += $(TS)
DIRS += $(EXTENSION_AR)
DIRS += $(CAU)
DIRS += $(VIEWIMAGE)
DIRS += $(YCA)
DIRS += $(CHANNELARCHIVER)
DIRS += $(SDDS)
DIRS += $(ADT)
DIRS += $(BEAMUTIL)
DIRS += $(BURT)
DIRS += $(CASR)
DIRS += $(CONDITION)
DIRS += $(KNOBCONFIG)
DIRS += $(OAGCA)
DIRS += $(SDDSEPICS)
DIRS += $(RAMPLOAD)
DIRS += $(SCOPEUTIL)
DIRS += $(SDDSIDL)
DIRS += $(STRIPTOOL)
DIRS += $(VDCT)

DIRS := $(wildcard $(DIRS))

$(NAMESERVER)_DEPEND_DIRS += $(GNUREGEX)

include $(TOP)/configure/RULES_DIRS

#
# =====================================================================================
# extensions dependent on SDDS           --  adt, burt, casr, ca, knobconfig, rampload,
#                                            condition, scopeutil, SDDSepics, oagca,
#                                            beamUtil, StripTool, sddsIDL
# extensions dependent on oagca          --  SDDSepics
# extensions dependent on ts             --  ar, cau
# extensions dependent on libdev (dalib) --  knobconfig, math, rampload
# extensions dependent on libLca (ca)    --  math, wingz
# extensions dependent on ezca           --  ca, scopeutil, ezcaIDL, EzcaScan, SDDS,
#                                            SDDSepics
# extensions dependent on EzcaScan       --  ezcaIDL
# extensions dependent on caObject       --  cajava
# extensions dependent on caObject2      --  histTool
# extensions dependent on jca            --  jprobe
# extensions dependent on dbStaticJava   --  Jdct
# extensions dependent on gnuregex       --  gateway
#
#
# =====================================================================================

# =====================================================================================
#       Other Extensions Dependencies
#       -----------------------------
#       X11 is needed by the following extensions
#               alh, adt, ar, burt, dp, mdct, km, libUnix, medm, probe, probeII,
#               snaps, stripTool, caTCL, sga, histTool, viewImage
#               motifButton, medm_cebaf, adt, popup, namecapture, SDDS/SDDSaps
#       Motif is needed by the following extensions
#               alh, adt, ar, burt, dp, km, libUnix, medm, probe, probeII, snaps,
#               StripTool motifButton, medm_cebaf, popup, sga, histTool
#               namecapture, SDDS/SDDSaps, viewImage, jpt
#       openwin3 is needed by the following extensions
#               ar/ar
#       Wingz is needed by the following extensions
#               wingz
#       Mathematica is needed by the following extensions
#               math
#       Xrtgraph is needed by the following extensions
#               medm, medm_cebaf
#       Interviews (Unidraw) is needed by the following extensions
#               gdct313
#       Tk-tcl is needed by the following extensions
#               ar/arrtk, caTCL, dct, casr
#       Blt is needed by the following extensions
#               ar/arrtk, caTCL
#       Idl is needed by the following extensions
#               ezcaIDL, idl
#       Java is needed by the following extensions
#               jca,cajava,jprobe,Jdct,dbStaticJava
#
# =====================================================================================

