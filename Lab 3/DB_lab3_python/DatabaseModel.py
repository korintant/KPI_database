import timeit

from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker
from sqlalchemy.sql import select
from tables.Education import Education
from tables.Education_document import Education_document
from tables.EIT_certificate import EIT_certificate
from tables.Individual import Individual
from tables.Student_ticket import Student_ticket
from tables.Subject_of_education_activity import Subject_of_education_activity
from tables.base import BaseModel
from db.DBSession import DBSession

from sqlalchemy.inspection import inspect
from sqlalchemy import delete
from sqlalchemy import insert
from sqlalchemy import update
from sqlalchemy import desc
from sqlalchemy import func

class DatabaseModel(object):
    session: DBSession
    inspect_:None
    engine:None
    session_factory:None

    tableIndex = -1
    typeIndex = 0

    TYPES = {
        'Education' : Education,
        'Education_document' : Education_document,
        'EIT_certificate' : EIT_certificate,
        'Individual' : Individual,
        'Student_ticket' : Student_ticket,
        'Subject_of_education_activity' : Subject_of_education_activity,
    }

    def __init__(self):
        self.engine = create_engine(
        f'postgresql://postgres:--@localhost:5432/edbo',
        pool_pre_ping=True
        )
        self.session_factory = sessionmaker(bind=self.engine)
        self.session = DBSession(self.session_factory())
        #self.session.query(Individual).all()

    def tablesBD(self):
        tables = inspect(self.engine).get_table_names()
        return tables

    def columns(self,table: str):
        inspect_mapper = inspect(self.TYPES[table])
        return inspect_mapper.columns.keys()

    def types(self,model: str):
        d = []
        table = self.TYPES[model]
        for c in table.__table__.columns:
            d.append(str(c.type))
        return d

    def primaryKey(self,table: str):
        inspect_mapper = inspect(self.TYPES[table])
        return inspect_mapper.primary_key[0].name

    def insertData(self):
        tables = self.tablesBD()
        print('Choose table:')
        tableIndex = 1
        for table in tables:
            print(tableIndex,table)
            tableIndex = tableIndex + 1
        tableIndex = int(input())
        #print('table: ',tables[tableIndex - 1])
        cols = self.columns(tables[tableIndex - 1])
        types = self.types(tables[tableIndex - 1])
        data = {}
        typeIndex = 0
        for col in cols:
            print('Insert value for column ',col)
            strType = types[typeIndex]
            val = type(strType)(input())
            typeIndex = typeIndex + 1
            data[col] = val
        emp = self.TYPES[tables[tableIndex - 1]](**data)
        self.session._session.add(emp)  
        self.session._session.commit()

    def deleteData(self):

        tables = self.tablesBD()
        print('Choose table:')
        tableIndex = 1

        for table in tables:
            print(tableIndex,table)
            tableIndex = tableIndex + 1
        tableIndex = int(input())

        print("Choose the value for column ID")

        types = self.types(tables[tableIndex - 1])
        cols = self.columns(tables[tableIndex - 1])

        val = int(input())
        self.session._session.query(self.TYPES[tables[tableIndex - 1]]).filter(self.TYPES[tables[tableIndex - 1]].ID == val).delete()
        self.session._session.commit()

    def updateData(self):
        tables = self.tablesBD()
        print('Choose table:')
        tableIndex = 1
        for table in tables:
            print(tableIndex,table)
            tableIndex = tableIndex + 1
        tableIndex = int(input())
        tableIndex = tableIndex - 1

        print("Choose the value for column ID")

        types = self.types(tables[tableIndex])
        cols = self.columns(tables[tableIndex])
 
        val = int(input())
        table = self.TYPES[tables[tableIndex]]
        data = {}
        typesIndex = 0
        for col in cols:
            strtype = types[typesIndex]
            typesIndex = typesIndex + 1
            print('Want to update column ',col,'? y/n')
            choose = str(input())
            if choose == 'y':
                print('Input value for column ',col)
                colValue = type(strtype)(input())
                data[col] = colValue
        item = self.session._session.query(table).filter(table.ID == val).one()
        for key in data:
            setattr(item,key,data[key])
        self.session._session.commit()

    def showData(self):
        tables = self.tablesBD()
        print('Choose table:')
        tableIndex = 1
        for table in tables:
            print(tableIndex,table)
            tableIndex = tableIndex + 1
        tableIndex = int(input())
        tableIndex = tableIndex - 1
        table = self.TYPES[tables[tableIndex]]
        rs = (self.engine.execute(select(table)))
        for row in rs:
            print(row)

