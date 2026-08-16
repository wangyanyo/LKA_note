/*
 *  linux/kernel/acct.c
 *
 *  BSD Process Accounting for Linux
 * 
 *  Author: Marco van Wieringen <mvw@planets.elm.net>
 * 
 *  Some code based one ideas and code from:
 *  Thomas K. Dyas <tdyas@eden.rutgers.edu>
 *  
 *  This file implements BSD-style process accounting. Whenever any
 *  process exists, an accounting record of type "struct acct" is
 *  written to the file specified with acct() system call. It is
 *  up to user-level programs to do useful things with the accounting
 *  log. The kernel just provides the raw accouting information.
 * 
 * 
 *  
 */