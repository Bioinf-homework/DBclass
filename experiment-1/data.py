#coding=utf-8
import random
def randdata():
	firstname = ["赵","钱","孙","李","周","吴","郑","王","张","钟","马","牛"]
	n1 = len(firstname)
	# for i in range(100):
	str(int(random.random()*(n1)))

	lastname = str(int(random.random()*100))
	data1= firstname[int(random.random()*n1)] + lastname

	data2 = str(int(random.random()*10**18))

	address = ["A","B","C"]
	data3 = address[int(random.random()*3)]+str(int(random.random()*3))

	# salary
	data4 = int(random.random()*5000)

	# sup
	data5 = str(int(random.random()*10**18))

	dno = ["D1","D2","D3","D4","D5"]
	n2 = len(dno)
	data6 = dno[int(random.random()*n2)]

	return (data1, data2, data3, data4, data5, data6)

def main():
	# pass
	fp = open("data.txt","w")
	for x in xrange(1,45):
		data = randdata()
		for d in data:
			fp.write(str(d))
			fp.write("\t")
		fp.write("\n")


if __name__ == '__main__':
	main()

# data = randdata()
# for d in data:
# 	print d