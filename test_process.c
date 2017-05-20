/* Copyright 2017 Eduardo Valle. All rights reserved.
   eduardovalle.com/ github.com/recodbr

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int MaxCount;
    int i;
    if (argc<=1 || strcmp(argv[1], "-h")==0 || strcmp(argv[1], "--help")==0) {
        fprintf(stderr, "usage: test_process <number of cycles> [ --ignore-sigterm ] [ dummy arguments... ]\n\n");
        return 1;
    }
    if (argc>2 && strcmp(argv[2], "--ignore-sigterm")==0) {
        printf("Ignoring SIGTERM.\n");
        signal(SIGTERM, SIG_IGN);
    }
    for (i=0; i<argc; i++) {
        printf("argv[%d]='%s'\n", i, argv[i]);
    }
    MaxCount = atoi(argv[1]);
    for (i=0; i<MaxCount; i++) {
        printf("%d ", i);
        fflush(stdout);
        sleep(5);
    }
    printf("\n");
    return 0;
}
