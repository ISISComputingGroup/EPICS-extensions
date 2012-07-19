TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
#  Optimization of db files using dbst (DEFAULT: NO)
#DB_OPT = YES

#----------------------------------------------------
# Create and install (or just install) into <top>/db
# databases, templates, substitutions like this
DB += galil_motor_withwrappers.template

DB += galil_motor_withoutwrappers.template

DB += galil_motor_extras.template

DB += galil_digital_in_bits.template
DB += galil_digital_in_bytes.template
DB += galil_digital_out_bits.template
DB += galil_digital_out_bytes.template

DB += galil_analog.template

DB += galil_userdef_records.template

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_template = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
install: 
	cp -f $(TOP)/galilSup/Db/galil_digital_out_bytes.req  $(TOP)/db/
	cp -f $(TOP)/galilSup/Db/galil_analog.req  $(TOP)/db/



