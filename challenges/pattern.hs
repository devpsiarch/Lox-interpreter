-- the equivilent of the visitor pattern in a functional 
-- programming language is doing "pattern matching" on a defined 
-- "algebric data type"

-- define a algebriac datatype 
data Expr 
  = Literal Int 
  | Binary Expr Expr 
  | Unary Expr 
-- define a function eval   

eval :: Expr -> Int

-- define the cases for calling this function 
-- called pattern match the input based on the values 
-- of the Expr

eval (Literal n) = n
eval (Binary r l) = eval r + eval l
eval (Unary :e) = - eval(e)



main :: IO()
main = do 
