# coding=utf-8
# Author: Tmn07
# Date: 2017-05-31

import json

# sql语句
sql1 = "SELECT [ ENAME = Mary & DNAME = Research ] ( EMPLOYEE JOIN DEPARTMENT )"
sql2 = "PROJECTION [ BDATE ] ( SELECT [ ENAME = John & DNAME = Research ] ( EMPLOYEE JOIN DEPARTMENT ) )"
sql3 = "SELECT [ ESSN2 = 01 ] (  PROJECTION [ ESSN2 , PNAME ] ( WORKS_ON JOIN PROJECT ) )"

# 基本关系
relations = ["EMPLOYEE", "DEPARTMENT", "PROJECT", "WORKS_ON"]
EMPLOYEE = ["ENAME", "ESSN", "ADDRESS", "SALARY", "SUPERSSN", "DNO", "BDATE"]
DEPARTMENT = ["DNAME", "DNO", "MGRSSN", "MGRSTARTDATE"]
PROJECT = ["PNAME", "PNO", "PLOCATION", "DNO"]
WORKS_ON = ["ESSN2", "PNO", "HOURS"]

keywords = ["SELECT", "PROJECTION"]
keyops = ["[", "]", "(", ")"]

# 基本关系操作

def join(relationA, relationB):
    return {"join ": [relationA, relationB]}


def SELECT(condition, relation):
    return {"SELECT [ " + condition + " ] ": relation}


def PROJECTION(condition, relation):
    return {"PROJECTION [ " + condition + " ] ": relation}



# 投影下推
def downprojection(node):
    nodes = node.keys()
    selnodes = [x for x in nodes if x.startswith("PROJECTION")]
    for sn in selnodes:
        sns = sn.split()
        conditions = []
        startcondition = 0
        endcondition = 0
        for i in range(len(sns)):
            if sns[i] == "[":
                startcondition = i
            if sns[i] == ",":
                endcondition = i
                conditions.append((startcondition, endcondition))
                startcondition = i
            if sns[i] == "]":
                endcondition = i
                conditions.append((startcondition, endcondition))

        for i in range(len(conditions)):
            condition = " ".join(sns[conditions[i][0] + 1:conditions[i][1]])
            relation = fromrelation(sns[conditions[i][0] + 1])
            if relation == "EMPLOYEE":
                condition+=" DNO"
            else:
                condition+=" PNO"
            prorelation(node, condition, relation)

    return node[sn]

# 选择下推
def downselect(node):
    nodes = node.keys()
    selnodes = [x for x in nodes if x.startswith("SELECT")]
    for sn in selnodes:
        sns = sn.split()
        conditions = []
        startcondition = 0
        endcondition = 0
        for i in range(len(sns)):
            if sns[i] == "[":
                startcondition = i
            if sns[i] == "&":
                endcondition = i
                conditions.append((startcondition, endcondition))
                startcondition = i
            if sns[i] == "]":
                endcondition = i
                conditions.append((startcondition, endcondition))
        for i in range(len(conditions)):
            condition = " ".join(sns[conditions[i][0] + 1:conditions[i][1]])
            relation = fromrelation(sns[conditions[i][0] + 1])
            selrelation(node, condition, relation)

    return node[sn]

# 查找字段所属关系
def fromrelation(condition):
    for r in relations:
        if condition in eval(r):
            return r

# 遍历树上关键词
def findkey(node):
    for d, x in node.items():
        for i in keywords:
            if d.startswith(i):
                if i == "SELECT":
                    node[d] = downselect(node)
                    node.setdefault('', node[d])
                    del (node[d])
                if i == "PROJECTION":
                    node[d] = downprojection(node)
                    node.setdefault('', node[d])
                    del (node[d])

        if type(x) == dict:
            findkey(x)
    return node

# 投影下推实现
def prorelation(node, condition, relation):
    for d, x in node.items():
        if d == "join ":
            for i in range(len(x)):
                if type(x[i]) == dict:
                    print x[i]
                    for k, v in x[i].items():
                        if v == relation:
                            x[i][k] = PROJECTION(condition, relation)
                if x[i] == relation:
                    x[i] = PROJECTION(condition, relation)

        if type(x) == dict:
            prorelation(x, condition, relation)

# 选择下推实现
def selrelation(node, condition, relation):
    for d, x in node.items():
        if d == "join ":
            for i in range(len(x)):
                if type(x[i]) == dict:
                    for k, v in x[i].items():
                        if v == relation:
                            x[i][k] = SELECT(condition, relation)
                if x[i] == relation:
                    x[i] = SELECT(condition, relation)

        if type(x) == dict:
            selrelation(x, condition, relation)

# 判断当前条件是否满足
def complete(ops):
    if ops[-4:] == ['[', ']', '(', ')']:
        del ops[-4:]
        return True
    else:
        return False

# 初始化逻辑计划树
def analysis(sql):
    res = {}
    words = sql.split()
    keys = []
    keyindex = []
    ops = []
    opindex = []
    n = len(words)

    for i in xrange(n):
        if words[i] == "JOIN":
            res[str((i - 1, i + 1))] = join(words[i - 1], words[i + 1])

    for i in xrange(n):
        if words[i] in keywords:
            keys.append(words[i])
            keyindex.append(i)
        if words[i] in keyops:
            key = keys[-1]
            ops.append(words[i])
            opindex.append(i)

            if complete(ops):
                condition = " ".join(words[opindex[-4] + 1: opindex[-2] - 1])
                if str((opindex[-2] + 1, opindex[-1] - 1)) in res.keys():
                    relation = res[str((opindex[-2] + 1, opindex[-1] - 1))]
                else:
                    relation = " ".join(words[opindex[-2]:opindex[-1] + 1])

                res[str((keyindex[-1], opindex[-1]))] = eval(key)(condition, relation)

                del opindex[-4:]

                tmp = {keys.pop(): [condition, relation]}
                keyindex.pop()
    lpt = res[str((0, n - 1))]
    with open("re.json", "w") as f:
        json.dump(lpt, f)
    print lpt
    return lpt

# 清除空节点
def clean(node):
    for d, x in node.items():
        if d == '':
            node = x
            if type(node) == dict:
                node = clean(node)
    return node

# 启发式优化
def optimization(lpt):
    node = findkey(lpt)
    node = clean(node)
    print node
    with open("re2.json", "w") as f:
        json.dump(node, f)


def main(sql):
    print "init Logical Query Plan Tree:"
    lpt = analysis(sql)
    print "after optimization:"
    optimization(lpt)


if __name__ == '__main__':
    main(sql2)
