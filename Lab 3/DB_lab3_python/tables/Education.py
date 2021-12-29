import enum
from sqlalchemy import Column, Integer, ForeignKey, Text, Numeric, Enum, Date
from tables.base import BaseModel

class Education_level(enum.Enum):
    professional_degree = 1
    bachelors_degree = 2
    masters_degree = 3
    educational_scientific_educational_creative_level = 4
    scientific_level = 5
    
class Education(BaseModel):
    __tablename__ = 'Education'
    Level_of_education = Column(Enum(Education_level),nullable = False)
    Education_program = Column(Text,nullable = False)
    