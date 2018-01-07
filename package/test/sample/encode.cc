#include <iostream>
#include <stdlib.h>
#include "RTIambImplementor.h"
using namespace std;

int main()
{
	struct buffer_t buf, temp;
	char **buffer_list;
	uint32_t *len_list;
	wstring str(L"This is a very long wstring"), str_temp;
	RTI_AttributeValue d, d1, d2;
	
	EndecodeFactory enc(1);
	
	RTIambImplementor a;

	int num = 1;
	double num2 = 5.2;
	RTI_AttributeValue var("Msg 1", 5);
	RTI_AttributeValue var1(&num, sizeof(int));
	RTI_AttributeValue var2(&num2, sizeof(double));
	RTI_ObjectClassHandle justtest(3);
	encode_RTI_ObjectClassHandle(justtest,&buf);
	
	
	enc.create(4);
	encode_RTI_AttributeValue(var, &buf);
	enc.add(&buf);
	encode_RTI_AttributeValue(var1, &buf);
	enc.add(&buf);
	encode_RTI_AttributeValue(var2, &buf);
	enc.add(&buf);
	encode_wstring(str, &buf);
	enc.add(&buf);
	enc.set_usertag(20);

	printf("Encode: type %d size %d count %d tag %d\n", enc.get_type(), enc.get_size(), enc.get_count(), enc.get_usertag());

	buffer_list = (char **)enc.get_buffer_list();
	len_list = enc.get_len_list();
	
	temp.start = buffer_list[1];
	temp.len = len_list[1];
	decode_RTI_AttributeValue(d, &temp);
	temp.start = buffer_list[2];
	temp.len = len_list[2];
	decode_RTI_AttributeValue(d1, &temp);
	temp.start = buffer_list[3];
	temp.len = len_list[3];
	decode_RTI_AttributeValue(d2, &temp);
	temp.start = buffer_list[4];
	temp.len = len_list[4];
	decode_wstring(str_temp, &temp);

	cout << (char *)d.data() << endl;
	cout << *(int *)d1.data() << endl;
	cout << *(double *)d2.data() << endl;
	wcout << str_temp << endl;

	return 0;
}

/*
 * vim: ts=4 sts=4 sw=4
 */
