NACHOS_DIR=../build/

cd $NACHOS_DIR
OUTPUT=$(./nachos-step2 -x test_exit_1 -d a| grep "Exit program, return code exit(1)")

if [ -n "$OUTPUT" ]; then
    exit 0
    else
        exit -1
	fi
