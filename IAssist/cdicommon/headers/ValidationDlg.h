
namespace CDICommon 
{
bool ValidateLong(long val, long min, long max, bool max_strict, bool min_strict, unsigned long param_name_id )
{
	bool valid = true;
	// validate for maximal value
	if(max_strict)
	{
		if(val>=max)
			valid = false;
	}
	else
	{
		if(val>max)
			valid = false ;
	}
	// validate for minimal value
	if(min_strict)
	{
		if(val<=min)
			valid = false;
	}
	else
	{
		if(val<min)
			valid = false ;
	}

	//show message
	if(!valid)
	{
		char szMsg[0x100]="";
		cdi_str str_msg_1 = LoadCdiStr(IDS_INVALID_PARAM_MSG_1);
		cdi_str str_msg_2 = LoadCdiStr(IDS_INVALID_PARAM_MSG_2);
		cdi_str str_param = LoadCdiStr(param_name_id);
		cdi_str str_msg = str_msg_1;
		str_msg+=cdi_str(" \"");
		str_msg+=str_param;
		str_msg+=cdi_str(" \" ");
		str_msg+=str_msg_2;
        str_msg+=cdi_str(" ");
		if(min_strict)
			sprintf( szMsg, "(%i...", min); 
		else	
			sprintf( szMsg, "[%i...", min); 
		str_msg+=cdi_str(szMsg);
		if(max_strict)
			sprintf( szMsg, "%i)", max); 
		else	
			sprintf( szMsg, "%i]", max); 
		str_msg+=cdi_str(szMsg);
		cdi_str str_cap = LoadCdiStr(IDS_ERROR);
		MessageBox(AfxGetMainWnd()->m_hWnd, str_msg.c_str(),str_cap.c_str(), MB_OK|MB_ICONERROR );
	}

	return valid;
}
bool ValidateDouble(double val, double min, double max, bool max_strict, bool min_strict, unsigned long param_name_id )
{
	bool valid = true;
	// validate for maximal value
	if(max_strict)
	{
		if(val>=max)
			valid = false;
	}
	else
	{
		if(val>max)
			valid = false ;
	}
	// validate for minimal value
	if(min_strict)
	{
		if(val<=min)
			valid = false;
	}
	else
	{
		if(val<min)
			valid = false ;
	}

	//show message
	if(!valid)
	{
		char szMsg[0x100]="";
		cdi_str str_msg_1 = LoadCdiStr(IDS_INVALID_PARAM_MSG_1);
		cdi_str str_msg_2 = LoadCdiStr(IDS_INVALID_PARAM_MSG_2);
		cdi_str str_param = LoadCdiStr(param_name_id);
		cdi_str str_msg = str_msg_1;
		str_msg+=cdi_str(" \"");
		str_msg+=str_param;
		str_msg+=cdi_str(" \" ");
		str_msg+=str_msg_2;
        str_msg+=cdi_str(" ");
		if(min_strict)
			sprintf( szMsg, "(%.2f...", min); 
		else	
			sprintf( szMsg, "[%.2f...", min); 
		str_msg+=cdi_str(szMsg);
		if(max_strict)
			sprintf( szMsg, "%.2f)", max); 
		else	
			sprintf( szMsg, "%.2f]", max); 
		str_msg+=cdi_str(szMsg);
		cdi_str str_cap = LoadCdiStr(IDS_ERROR);
		MessageBox(AfxGetMainWnd()->m_hWnd, str_msg.c_str(),str_cap.c_str(), MB_OK|MB_ICONERROR );
	}

	return valid;
}
}