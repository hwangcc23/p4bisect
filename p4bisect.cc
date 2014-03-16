/*
 * p4bisect.c
 *
 * p4bisect provides the equivalent to git-bisect for Perforce(P4).
 *
 * Copyright (C) 2014 Chih-Chyuan Hwang (hwangcc@csie.nctu.edu.tw)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

# include <clientapi.h>

int main(int argc, char **argv)
{
	ClientUser ui;
	ClientApi client;
	StrBuf msg;
	Error e;

	fprintf(stdout, "p4bisect - Find by binary search the change that introduced a bug\n");

	// Any special protocol mods

	// client.SetProtocol("tag");

	// Connect to server

	client.Init(&e);

	if (e.Test()) {
		e.Fmt( &msg );
		fprintf(stderr, "%s\n", msg.Text());
		return 1;
	}

	// Run the command "argv[1] argv[2...]"

	client.SetArgv(argc - 2, argv + 2);
	client.Run(argv[1], &ui);

	// Close connection

	client.Final(&e);

	if (e.Test()) {
		e.Fmt(&msg);
		fprintf(stderr, "%s\n", msg.Text());
		return 1;
	}
	
	return 0;
}
