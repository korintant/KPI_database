from sqlalchemy import Column, Integer, ForeignKey, Text, Numeric, Enum, Date
from tables.base import BaseModel

    
class Individual(BaseModel):
    __tablename__ = 'Individual'
    Name = Column(Text,nullable = False)

    