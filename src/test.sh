#!/bin/bash

# Set this to 1 if you want to do non obligatory tests
if [ ! "$DO_OBLIGATORY" ]
then
	DO_OBLIGATORY="0"
fi

do_eval()
{
	eval $* >/dev/null 2>/dev/null
	if [ $? == "0" ]; then
		echo "   SUCCESS"
	else
		echo "   FAILED"
		echo "DETAILED CALL: "
		echo "  $*"
		echo "OUTPUT: "
		eval $*
		exit 1;
	fi
}

call_test ()
{
	printf "$1"
	shift
	OBLIGATORY=$1
	shift
	
	if [ $OBLIGATORY != 0 ]; then
		do_eval $*
	else
		if [ $DO_OBLIGATORY != 0  ]; then
			do_eval $*
		else
			echo "   NOT OBLIGATORY, SKIPPING..."
		fi
	fi
}

call_test "Testing core/create (1), core/all_equals" 1 "core/create 8 8 1 | core/all_equals 1"
call_test "Testing core/create (2)" 1 "core/create 8 8 | core/all_equals 0"
call_test "Testing math/add" 1 "core/create 2 2 0 | math/add 0 1 2 3 | core/all_equals 1"

call_test "Testing the math/coords script" 1 "core/create 2 2 0 | math/add `./math/coords 2 0 0` `./math/coords 2 0 1` `./math/coords 2 1 0` `./math/coords 2 1 1` | core/all_equals 1"

call_test "Testing io/scat" 1 "core/create 8 8 1| io/scat | core/all_equals 1"
call_test "Testing io/field_to_seq, io/seq_to_field" 1 "core/create 8 8 8| io/field_to_seq | io/seq_to_field 8 8 | core/all_equals 8"

call_test "Testing math/equation for rows (1)" 1 "core/create 4 4 1 | math/equation 'x>=2&&x<=1' | core/all_equals 0"
call_test "Testing math/equation for rows (2)" 1 "core/create 4 4 1 | math/equation 'x>=0&&x<=3'  | core/all_equals 1"
call_test "Testing math/equation for cols (1)" 1 "core/create 4 4 1 | math/equation 'y>=2&&y<=1'  | core/all_equals 0"
call_test "Testing math/equation for cols (2)" 1 "core/create 4 4 1 | math/equation 'y>=0&&y<=3'  | core/all_equals 1"
call_test "Testing math/equation for valmin (1)" 1 "core/create 4 4 1 | math/equation 'v>=1' | core/all_equals 1"
call_test "Testing math/equation for valmin (2)" 1 "core/create 4 4 1 | math/equation 'v>=2' | core/all_equals 0"
call_test "Testing math/equation for valmax (1)" 1 "core/create 4 4 1 | math/equation 'v<=1' | core/all_equals 1"
call_test "Testing math/equation for valmax (2)" 1 "core/create 4 4 1 | math/equation 'v<=0' | core/all_equals 0"
call_test "Testing math/equation for valeq (1)" 1 "core/create 4 4 1 | math/equation 'v==1' | core/all_equals 1"
call_test "Testing math/equation for valeq (2)" 1 "core/create 4 4 1 | math/equation 'v==0' | core/all_equals 0"

call_test "Testing io/avalanches_bin2human" 1 "printf \"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x01\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x06\x00\x00\x00\x09\x00\x00\x00\x0a\x00\x00\x00\"  | io/avalanches_bin2human 2 | io/seq_to_field 2 2 | core/all_equals 1"
call_test "Testing io/avalanches_bin2human with ids" 1 "printf \"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x01\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x09\x00\x00\x00\x0a\x00\x00\x00\"  | io/avalanches_bin2human 2 ids | io/seq_to_field 2 2 | core/all_equals 1"

call_test "Testing algo/fix s" 1 "core/create 4 4 8 | algo/fix s | core/all_equals 2"
call_test "Testing algo/fix s hint" 1 "core/create 4 4 8 | algo/fix s 0 | core/all_equals 2"
call_test "Testing algo/fix s hint" 1 "core/create 4 4 8 | algo/fix s 15 | core/all_equals 2"
call_test "Testing algo/fix l" 1 "core/create 4 4 10 | algo/fix l | io/avalanches_bin2human 4 | io/seq_to_field 4 4 | algo/fix s | core/all_equals 3"

EQ_3_P_1='((x==y||x==8-y)&&v==1)||(x==4&&y==4&&v==0)||(x!=y&&x!=8-y&&v==3)'

call_test "Testing algo/fix s (2)" 1 "core/create 9 9 3 | math/add `./math/coords 9 4 4` | algo/fix s | math/equation \$EQ_3_P_1 | core/all_equals 1"
call_test "Testing algo/fix l (2)" 1 "core/create 9 9 3 | math/add `./math/coords 9 4 4` | algo/fix l `./math/coords 9 4 4` | io/avalanches_bin2human 9 | io/seq_to_field 9 9  | math/equation 'v-min(min(x+1,9-x),min(y+1,9-y))' | core/all_equals 0"
call_test "Testing algo/fix (special)" 1 "core/create 3 3 4 | algo/relax s `./math/coords 3 1 1` 0 | core/all_equals 4"

call_test "Testing algo/relax s" 1 "core/create 9 9 3 | math/add `./math/coords 9 4 4` | algo/relax s | math/equation \$EQ_3_P_1 | core/all_equals 1"
call_test "Testing algo/relax l" 1 "core/create 9 9 3 | math/add `./math/coords 9 4 4` | algo/relax l `./math/coords 9 4 4` | io/avalanches_bin2human 9 | io/seq_to_field 9 9  | math/equation 'v-min(min(x+1,9-x),min(y+1,9-y))' | core/all_equals 0"

call_test "Testing math/calc (1)" 1 "[ `echo 0 | math/calc '!0&&1==1&&1!=0&&1>=1&&1<=1&&!(1<1)&&!(1>1)&&1+1==+2&&1-4==-3&&8%3==2&&2*2==4&&9/3==3&&(0||1)==1&&(0||0)==0&&min(3,2)==2&&min(2,3)==2&&max(2,3)==3&&max(3,2)==3'` == '1' ]"
call_test "Testing math/calc (2)" 1 "core/create 2 2 0 | math/add 0 1 2 | io/field_to_seq | math/calc 'x+1' | io/seq_to_field 2 2 | math/add 0 | core/all_equals 1"
call_test "Testing math/calc (3)" 1 "[ `echo 0 | math/calc 'x?42:2015'` == '2015' ]"
call_test "Testing math/calc (4)" 1 "[ `echo 1 | math/calc 'x?42:2015'` == '42' ]"

call_test "Testing math/comb add" 1 "core/create 2 2 1 | math/comb add \"core/create 2 2 2\" | core/all_equals 3"
call_test "Testing math/comb sub" 1 "core/create 2 2 1 | math/comb sub \"core/create 2 2 2\" | core/all_equals -1"
call_test "Testing math/comb min" 1 "core/create 2 2 1 | math/comb min \"core/create 2 2 2\" | core/all_equals 1"
call_test "Testing math/comb max" 1 "core/create 2 2 1 | math/comb max \"core/create 2 2 2\" | core/all_equals 2"

call_test "Testing algo/id" 1 "core/create 64 64 3 | math/comb add \"algo/id 64 64\" | algo/fix s | core/all_equals 3"

#call_test "Testing algo/S" 1 "core/create 9 9 3 | math/add `./math/coords 9 4 4` | algo/S | math/equation \$EQ_3_P_1 | core/all_equals 1"
#call_test "Testing algo/L" 1 "core/create 9 9 3 | math/add `./math/coords 9 4 4` | algo/L 9 `./math/coords 9 4 4` | io/seq_to_field 9 9  | math/equation 'v-min(min(x+1,9-x),min(y+1,9-y))' | core/all_equals 0"

#call_test "Testing algo/s" 1 "core/create 9 9 3 | math/add `./math/coords 9 4 4` | algo/s | math/equation \$EQ_3_P_1 | core/all_equals 1"
#call_test "Testing algo/l" 1 "core/create 9 9 3 | math/add `./math/coords 9 4 4` | algo/l 9 `./math/coords 9 4 4` | io/seq_to_field 9 9  | math/equation 'v-min(min(x+1,9-x),min(y+1,9-y))' | core/all_equals 0"
call_test "Testing algo/throw" 1 "core/create 9 9 3 | algo/throw `./math/coords 9 4 4` | math/equation \$EQ_3_P_1 | core/all_equals 1"

call_test "Testing algo/burning_test" 1 "core/create 2 2 3 | algo/burning_test | io/avalanches_bin2human 2 | io/seq_to_field 2 2 | core/all_equals 1"
call_test "Testing algo/is_recurrent (1)" 1 "[ `core/create 10 10 2 | algo/is_recurrent` == 'recurrent' ]"
call_test "Testing algo/is_recurrent (2)" 1 "[ `core/create 10 10 1 | algo/is_recurrent` == 'transient' ]"

call_test "Testing algo/random_throw (input)" 1 "core/create 9 9 3 | math/add `./math/coords 9 4 4` | io/field_to_seq | algo/random_throw input 9 9 s | math/equation \$EQ_3_P_1 | core/all_equals 1"
call_test "Testing algo/random_throw (random)" 1 "core/create 9 9 0 | algo/random_throw random 1 42 | math/equation 'v<=1' | core/all_equals 1"

call_test "Testing io/to_tga (0=green, 3=red)" 1 "algo/id 50 50 | io/to_tga 00ff00 ff0000 > /dev/null"

call_test "Testing algo/super (1)" 1 "core/create 2 2 2 | algo/super | core/all_equals 0"
call_test "Testing algo/super (2)" 1 "core/create 2 2 3 | algo/super | core/all_equals 1"

# ca
call_test "Testing ca/ca (1)" 1 "core/create 20 20 0 | ca/ca 'v:=v+2' end 4 | core/all_equals 8"
call_test "Testing ca/ca (2)" 1 "echo '0 1 0 0 1 0 1 0 0 0 1 1 0 0' | ca/ca 'v:=(a[1,0]>=0)?(a[1,0]):0' end 1 | core/diff2 'echo 1 0 0 1 0 1 0 0 0 1 1 0 0 0'"
call_test "Testing ca/ca (3)" 1 "core/create 20 20 4 | ca/ca 'v:=v+(-4*(v>=4))+(a[-1,0]>=4)+(a[0,-1]>=4)+(a[1,0]>=4)+(a[0,1]>=4)' | core/diff2 'core/create 20 20 4 | algo/S'"

# rotor stuff
#call_test "Testing rotor/rotor s" 1 "core/create 10 10 0 | rotor/rotor s 'core/create 10 10 100' | core/diff2 \"core/create 10 10 0 | algo/S | rotor/rotor s 'core/create 10 10 100'\""
#call_test "Testing rotor/rotor l" 1 "core/create 2 2 0 | math/equation 'min(x+y*2,2)' | rotor/rotor l 'core/create 2 2 0 | math/add 0' | io/avalanches_bin2human 2 | io/seq_to_field 2 2 | core/all_equals 1"
#call_test "Testing io/convert" 1 "core/create 3 3 3 | io/convert numbers rotors | io/convert rotors numbers | core/all_equals 3"
#call_test "Testing rotor/xrotor" 1 "core/create 3 3 3 | io/convert numbers rotors | rotor/xrotor s 'core/create 3 3 0' | io/convert rotors numbers | core/all_equals 3"

# transition functions
call_test "Testing ca/transf_by_grids " 1 "cat ../../data/ca_by_grid/circuit.txt  | ca/transf_by_grids | diff ../../data/ca/circuit.txt -"

# scripts
call_test "Testing math/add2" 1 "core/create 2 2 1 | math/add2 \"core/create 2 2 2\" | core/all_equals 3"
call_test "Testing math/sub2" 1 "core/create 2 2 1 | math/sub2 \"core/create 2 2 2\" | core/all_equals -1"
call_test "Testing core/diff2" 1 "core/create 9 9 3 | core/diff2 core/create 9 9 3"

# test binary
# call_test "Testing test/test" 1 "test/tests"

# obligatory
call_test "Testing io/to_ods" 0 "core/create 9 4 4 | io/to_ods tmp"

