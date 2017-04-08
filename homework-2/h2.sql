create view shp as select jno, jname from j where city = "上海";

select pno from s, p, spj where spj.sno = s.sno and spj.pno = p.pno and s.city = j.city;

select jno
from spj
where jno not in (
	select jno from p,spj,s where 
	s.city="天津" and 
	p.color = "red" and  
	spj.sno = s.sno and 
	spj.pno = p.pno 
	)

select distinct sno from spj where pno in (
	select pno from p where weight = (select min(weight) from p)
);

select pno from p
minus
select pno from spj, j where spj.jno = j.jno and j.city = "北京"

select jname, sum(qty) from spj, j where spj.jno=p.jno group by jno order by jno ASC


select jname, sum(qty) aw from spj, j where spj.jno=p.jno group by jno having aw > 1000;

update p set color = "blue" where color = "red";

insert into spj values("S2","P4","J6",200);

delete from s where sno="S2";
delete from spj where sno ="S2";

#2
select cid, cname from c where teacher="王老师";

select sid, sname from s where sex="男" and age>22;

select cname, teacher from sc, c where sc.cid=c.cid and sc.sid=10001;

select distinct sname from sc, s, c 
	where sc.sid=s.sid and sc.cid=c.cid and sex="女" and teacher="王老师";

select cid from c
minus
select cid from sc, s where sc.sid=s.sid and sname="张小明";

select sc.sid, sname from sc,s 
	where sc.sid=s.sid 
	group by sc.sid 
	having count(*)>1

select s.sid, sname from sc, s 
	where sc.sid=s.sid
	group by sc.sid 
	having avg(grade)>85

select cid, cname from c, sc
	where c.cid=sc.cid 
	group by sc.cid
	having count(*) = (
		select count(*) from s;
		)


#3
#3.1
create table Orders
(
	orderid int not null,
	Supplierid int not null,
	Movieid int not null,
	copies int not null,
	Primary key(orderid)
);

create table Rentals
(
	Customerid int not null,
	Tapelid int not null,
	ckoutDate datetime not null,
	Duration int not null,
	Primary key(Customerid, Tapelid)
);

#3.2
insert into Rentals values(2398, 5600, "2017-3-26", 30)

delete from Rentals WHERE  ckoutDate<'2000-01-01';

update MovieSupplier set price = price/6.88;

#3.3

create view jhvs select 
	select MovieName, Price
	from Moives, MovieSupplier
	where Supplierid in (
		select Supplierid
		from MovieSupplier, Moives
		where Moives.Movieid=MovieSupplier.Movieid
			and Moives.MovieName="Joe's House of Video"
		)

#3.4

select SupplierName, count(*)
	from Suppliers, MovieSupplier
	where Suppliers.Supplierid = MovieSupplier.Supplierid
	group by Suppliers.Supplierid;

#3.5

select MovieName
	from Orders, Moives
	where Orders.Movieid=Moives.Movieid
		and Orders.copies>5;

#3.6

select MovieName
	from Inventory, Moives
	where Moives.Movieid=Inventory.Movieid
	group by Inventory.Movieid
	having count(*) > 1;

#3.7

select MovieName
	from Rentals, Inventory, Moives
	where Rentals.Tapelid=Inventory.Tapelid
		and Inventory.Movieid=Moives.Movieid
		and Duration = (
				select max(Duration) from Rentals;
			)

#3.8
select MovieName
	from Moives
	where Movieid in (
			select Movieid from Moives 
			minus 
			select distinct Movieid from Inventory
		)

#3.9
select Supplierid, SupplierName
	from Suppliers, MovieSupplier
	where Suppliers.Supplierid=MovieSupplier.Supplierid
		and price = (
				select min(price) 
				from MovieSupplier, Moives
				where MovieName="Hacksaw Ridge"
					and MovieSupplier.Movieid=Moives.Movieid
			)

#3.10

select FirstName, LastName 
	from Customers, Rentals, Inventory, Moives
	where Customers.Customerid = Inventory.Customerid
		and Rentals.Tapelid=Inventory.Tapelid
		and Inventory.Movieid=Moives.Movieid
		and (
			MovieName="Beauty and the Beast"
			or
			Movieid in (
					select Movieid
					from Suppliers, MovieSupplier
					where Suppliers.Supplierid=MovieSupplier.Supplierid
						and Suppliers.SupplierName="VWS Video"
				)
		)



