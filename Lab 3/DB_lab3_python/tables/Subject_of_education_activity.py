import enum
from sqlalchemy import Column, Integer, ForeignKey, Text, Numeric, Enum, Date
from tables.base import BaseModel

class Ukraine_region(enum.Enum):
    Cherkasy_Oblast = 1
    Chernihiv_Oblast = 2
    Chernivtsi_Oblast = 3
    History_of_Ukraine = 4
    Dnipropetrovsk_Oblast = 5
    Donetsk_Oblast = 6
    Ivano_Frankivsk_Oblast = 7
    Kharkiv_Oblast = 8
    Kherson_Oblast = 9
    Khmelnytskyi_Oblast = 10
    Kyiv_Oblast = 11
    Kirovohrad_Oblast = 12
    Luhansk_Oblast = 13
    Lviv_Oblast = 14
    Mykolaiv_Oblast = 15
    Odessa_Oblast = 16
    Poltava_Oblast = 17
    Rivne_Oblast = 18
    Sumy_Oblast = 19
    Ternopil_Oblast = 20
    Vinnytsia_Oblast = 21
    Volyn_Oblast = 22
    Zakarpattia_Oblast = 23
    Zaporizhzhia_Oblast = 24
    Zhytomyr_Oblast = 25
    Kyiv = 26

class Subject_of_education_activity(BaseModel):
    __tablename__ = 'Subject_of_education_activity'
    Name = Column(Text,nullable = False)
    Region = Column(Enum(Ukraine_region),nullable = False)
    Type = Column(Integer,nullable = False)
    