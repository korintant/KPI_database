from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker

from db.DBSession import DBSession

from DatabaseModel import DatabaseModel


model = DatabaseModel()

option = '1'
while option != "0":
    option = input("0 - exit\n1 - insert\n2 - delete\n3 - update\n4 - display\nChoose an option: ")
    if option == "1":
        model.insertData()
    elif option == "2":
        model.deleteData()
    elif option == "3":
        model.updateData()
    elif option == "4":
        model.showData()
    elif option != "0":
        print("Wrong option!")
