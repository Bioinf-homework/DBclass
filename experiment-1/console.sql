DESCRIBE employee;
DESCRIBE department;
DESCRIBE project;
DESCRIBE works_on;

## 1
SELECT ENAME
FROM employee, works_on, project
WHERE project.PNO = works_on.PNO AND employee.ESSN = works_on.ESSN AND project.PNAME = "SQLProject";

## 2
SELECT
  ENAME,
  ADDRESS
FROM employee, department
WHERE employee.DNO = department.DNO AND department.DNAME = "Research Department" AND SALARY < 3000;

## 3
SELECT ENAME
FROM employee
WHERE ESSN NOT IN (
  SELECT ESSN
  FROM works_on
  WHERE PNO = "P1"
);

## 4
SELECT
  ENAME,
  DNAME
FROM employee, department
WHERE employee.DNO = department.DNO
      AND SUPERSSN = (
  SELECT ESSN
  FROM employee
  WHERE ENAME = "张红"
);

## 5
SELECT EMPLOYEE.ESSN
FROM EMPLOYEE, works_on
WHERE employee.ESSN = works_on.ESSN AND works_on.PNO = 'P2' AND employee.ESSN IN (
  SELECT ESSN
  FROM works_on
  WHERE works_on.PNO = 'P1');

## 6
SELECT
  works_on.ESSN,
  ENAME
FROM employee, works_on
WHERE employee.ESSN = works_on.ESSN
GROUP BY works_on.ESSN
HAVING count(*) = (SELECT count(*)
                   FROM project);

## 7
SELECT DNAME
FROM department, employee
WHERE department.DNO = employee.DNO
GROUP BY department.DNO
HAVING avg(SALARY) < 3000;

## 8
SELECT ENAME
FROM employee, works_on
WHERE employee.ESSN = works_on.ESSN
GROUP BY works_on.ESSN
HAVING sum(HOURS) < 8 AND count(*) > 2;

## 9
SELECT
  dno,
  sum(SALARY) / sum(HOURS)
FROM EMPLOYEE, works_on
WHERE employee.ESSN = works_on.ESSN
GROUP BY DNO;