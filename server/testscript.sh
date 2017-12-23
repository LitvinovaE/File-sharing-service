#!/bin/bash

cd ~/dev/zashitka/
openssl s_client \
	-cert testchild/child.crt \
	-key testchild/child.key \
	-pass file:/tmp/pass \
	-CAfile rootCA.d/rootCA.crt \
	-crlf \
	-connect "$@"

