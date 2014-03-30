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
#include "p4bisect.h"

void P4BisectClientUser::Message(Error *err)
{
	StrBuf sb;

	err->Fmt(sb, EF_PLAIN);

	if (p4bisect) {
		if (!p4bisect->SyncingFile(sb)) {
			p4bisect->AddRevision(sb);
		}
	}
}

void P4BisectClientUser::OutputError(const char *errBuf)
{
	StrBuf sb(errBuf);

	if (p4bisect) {
		p4bisect->SyncingFile(sb);
	}
}
