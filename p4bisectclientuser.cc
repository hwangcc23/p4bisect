/*
 * p4bisectclientuser.cc
 *
 * p4bisect provides the equivalent to git-bisect for Perforce(P4).
 *
 * Copyright (C) 2014 Chih-Chyuan Hwang (hwangcc@csie.nctu.edu.tw)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "p4bisectclientuser.h"

void P4BisectClientUser::Message(Error *err)
{
	StrBuf buf;

	err->Fmt(buf, EF_PLAIN);
	
	//fprintf(stdout, "%s\n", buf.Text());
}
