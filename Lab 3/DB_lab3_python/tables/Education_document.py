from sqlalchemy import Column, Integer, ForeignKey, Text, Numeric, Enum, Date
from tables.base import BaseModel

    
class Education_document(BaseModel):
    __tablename__ = 'Education_document'
    Education_ID = Column(Integer,ForeignKey('Education.ID', ondelete='CASCADE'),nullable = False)
    Individual_ID = Column(Integer,ForeignKey('Individual.ID', ondelete='CASCADE'),nullable = False)
    Subject_ID = Column(Integer,ForeignKey('Subject.ID', ondelete='CASCADE'),nullable = False)
    GPA = Column(Numeric(5,2),nullable = False)
    Received_date = Column(Date,nullable = False)
    