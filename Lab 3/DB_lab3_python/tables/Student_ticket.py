from sqlalchemy import Column, Integer, ForeignKey, Text, Numeric, Enum, Date
from tables.base import BaseModel

    
class Student_ticket(BaseModel):
    __tablename__ = 'Student_ticket'
    Individual_ID = Column(Integer,ForeignKey('Individual.ID', ondelete='CASCADE'),nullable = False)
    Subject_ID = Column(Integer,ForeignKey('Subject.ID', ondelete='CASCADE'),nullable = False)
    Start_date = Column(Date,nullable = False)
    End_date = Column(Date,nullable = False)
    Education_ID = Column(Integer,ForeignKey('Education.ID', ondelete='CASCADE'),nullable = False)
    