import enum
from sqlalchemy import Column, Integer, ForeignKey, Text, Numeric, Enum, Date
from tables.base import BaseModel

class EIT_subject(enum.Enum):
    Ukrainian_language_and_literature = 1
    Ukrainian_language = 2
    Math = 3
    History_of_Ukraine = 4
    Geography = 5
    Biology = 6
    Physics = 7
    Chemistry = 8
    English = 9
    German = 10
    French = 11
    Spanish = 12

class EIT_certificate(BaseModel):
    __tablename__ = 'EIT_certificate'
    Individual_ID = Column(Integer,ForeignKey('Individual.ID', ondelete='CASCADE'),nullable = False)
    Year = Column(Integer,nullable = False)
    Subject = Column(Enum(EIT_subject),nullable = False)
    Result = Column(Numeric(5, 2),nullable = False)
