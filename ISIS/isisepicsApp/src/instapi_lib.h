#ifndef INSTAPI_LIB_H
#define INSTAPI_LIB_H

#ifdef INSTAPI_LIB_EXPORTS
#define INSTAPI_DLL __declspec(dllexport)
#else
#define INSTAPI_DLL __declspec(dllimport)
#endif

class INSTAPI_DLL InstAPILib
{
private:
	typedef std::map<ULONG, std::string> session_map_t;
	session_map_t m_session; 
	void accessRuncontrol(ULONG session_id, BSTR block_name, VARIANT_BOOL new_value, VARIANT_BOOL* value, bool setvalue);
	void accessLimits(ULONG session_id, BSTR block_name, VARIANT new_value, BSTR* value, bool setvalue, bool low);
public:
	static COAUTHIDENTITY* createIdentity(const std::string& user, const std::string&  domain, const std::string& pass);
	static HRESULT setIdentity(COAUTHIDENTITY* pidentity, IUnknown* pUnk);
	HRESULT test();
	HRESULT testicp();
	HRESULT testseci();
	HRESULT testlv();
	// session functions
	void createSession(BSTR instrument, BSTR user_name, BSTR password, ULONG* session_id);
	void deleteSession(ULONG session_id);
	void getRole(ULONG session_id, BSTR* role);
	void setRole(ULONG session_id, BSTR role);
	void putMessage(ULONG session_id, BSTR time, BSTR severity, BSTR message);
	void getMessages(ULONG session_id, LONG start_index, BSTR min_severity, VARIANT* times, VARIANT* severities, VARIANT* messages, LONG* end_index);
	void getBlockNames(ULONG session_id, VARIANT* block_names);
	// block functions
	void getBlockValue(ULONG session_id, BSTR block_name, VARIANT* block_value);
	void getBlockSetpoint(ULONG session_id, BSTR block_name, VARIANT* setpoint_value);
	void sendBlockSetpoint(ULONG session_id, BSTR block_name, VARIANT setpoint_value, bool push_button);
	void getRunControl(ULONG session_id, BSTR block_name, VARIANT_BOOL* rc_value);
	void setRunControl(ULONG session_id, BSTR block_name, VARIANT_BOOL rc_value);
	void getRunControlLimit(ULONG session_id, BSTR block_name, VARIANT* limit_value, bool low);
	void setRunControlLimit(ULONG session_id, BSTR block_name, VARIANT new_value, bool low);
	void pushBlockButton(ULONG session_id, BSTR block_name);
	// labview functions
	void getLabviewValue(ULONG session_id, BSTR vi_name, BSTR control_name, VARIANT* value);
	void setLabviewValue(ULONG session_id, BSTR vi_name, BSTR control_name, VARIANT value);
	void callLabview(ULONG session_id, BSTR vi_name, VARIANT names, VARIANT values, VARIANT_BOOL reentrant, VARIANT* results);
	void toggleViVisibility(ULONG session_id, BSTR vi_name, VARIANT_BOOL show);
	void getViState(ULONG session_id, BSTR vi_name, VARIANT* value);
	void startVi(ULONG session_id, BSTR vi_name);
	void stopVi(ULONG session_id, BSTR vi_name);
	void closeViFrontPanel(ULONG session_id, BSTR vi_name);
	void getViNames(ULONG session_id, VARIANT* names);
	// dae functions
	void getRunState(ULONG session_id, BSTR* messages, LONG* status);
	void beginRun(ULONG session_id, LONG period, VARIANT_BOOL wait, VARIANT_BOOL quiet, VARIANT_BOOL paused, BSTR* messages);	
	void abortRun(ULONG session_id, BSTR* messages);
	void endRun(ULONG session_id, BSTR* messages);
	void pauseRun(ULONG session_id, BSTR* messages);
	void resumeRun(ULONG session_id, BSTR* messages);
	void recoverRun(ULONG session_id, BSTR* messages);
	void saveRun(ULONG session_id, BSTR* messages);
	void updateCRPT(ULONG session_id, BSTR* messages);
	void storeCRPT(ULONG session_id, BSTR* messages);
	void getGoodFrames(ULONG session_id, VARIANT_BOOL curr_period, BSTR* messages, LONG* frames);
	void getUamps(ULONG session_id, VARIANT_BOOL curr_period, BSTR* messages, DOUBLE* uamps);
	void getMEvents(ULONG session_id, BSTR* messages, DOUBLE* mev);
	void getCurrentPeriod(ULONG session_id, BSTR* messages, LONG *period, LONG* daq_period);
	void getRunNumber(ULONG session_id, BSTR* messages, LONG* run_no);
	void getTotalCounts(ULONG session_id, BSTR* messages, LONG* counts);
	void snapshotCRPT(ULONG session_id, BSTR filename, LONG do_update, LONG do_pause, BSTR* messages);
	void sumAllHistogramMemory(ULONG session_id, BSTR* messages, LONG* counts);
	void sumAllSpectra(ULONG session_id, BSTR* messages, LONG* counts, LONG* bin_counts);
	void setPeriod(ULONG session_id, LONG period_number, BSTR* messages);
	void getSpectrum(ULONG session_id, LONG spectrum_no, LONG period, VARIANT* time_channels, VARIANT* signal, LONG as_dist, LONG* sum, BSTR* messages);
	void changeNumberOfSoftwarePeriods(ULONG session_id, LONG num_periods, BSTR* messages);
	void vmeWriteValue(ULONG session_id, ULONG card_id, ULONG card_address, ULONG word_size, ULONG value, ULONG mode, BSTR* messages);
	//Others
	void setRbNumber(ULONG session_id, VARIANT rb_no);
	void getRbNumber(ULONG session_id, VARIANT* rb_no);
	void setTitle(ULONG session_id, VARIANT title);
	void setBeamlineParameter(ULONG session_id, VARIANT name, VARIANT value, VARIANT units);
	void deleteBeamlineParameter(ULONG session_id, VARIANT name);
	void deleteAllBeamlineParameters(ULONG session_id);
	void getBeamlineParameter(ULONG session_id, BSTR name, BSTR* value, BSTR* units, BSTR* type);
	void getBeamlineParameterNames(ULONG session_id, VARIANT* names);
	void setSampleParameter(ULONG session_id, VARIANT name, VARIANT value, VARIANT units);
	void deleteSampleParameter(ULONG session_id, VARIANT name);
	void deleteAllSampleParameters(ULONG session_id);
	void getSampleParameter(ULONG session_id, BSTR name, BSTR* value, BSTR* units, BSTR* type);
	void getSampleParameterNames(ULONG session_id, VARIANT* names);
	void getSeciConfig(ULONG session_id, BSTR* name);
	void clearDatabaseLogs(ULONG session_id, int run_num);
	void clearLogFiles(ULONG session_id, int run_num, BSTR path);
	void getMeasurementLabel(ULONG session_id, BSTR id, BSTR* label);
	void setMeasurementLabel(ULONG session_id, BSTR id, BSTR label);
	void getMeasurementId(ULONG session_id, BSTR label, VARIANT_BOOL newId, BSTR* id);
};


#endif /* INSTAPI_LIB_H */
