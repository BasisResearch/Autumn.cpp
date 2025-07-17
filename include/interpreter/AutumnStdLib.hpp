#ifndef AUTUMN_STD_LIB_HPP_
#define AUTUMN_STD_LIB_HPP_
#include "AutumnCallable.hpp"
#include "AutumnCallableValue.hpp"
#include "AutumnValue.hpp"
#include <memory>
#include <vector>

namespace Autumn {

class RenderAll : public AutumnCallable,
                 public std::enable_shared_from_this<RenderAll> {
public:
  RenderAll() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 0; }
  std::string toString() const override { return "<native fn: RenderAll>"; }
  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class Defined : public AutumnCallable,
               public std::enable_shared_from_this<Defined> {
public:
  Defined() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 1; }
  std::string toString() const override { return "<native fn: Defined>"; }
  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class IsFreePos : public AutumnCallable,
                 public std::enable_shared_from_this<IsFreePos> {
public:
  IsFreePos() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 1; }
  std::string toString() const override { return "<native fn: IsFreePos>"; }
  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class Print : public AutumnCallable,
             public std::enable_shared_from_this<Print> {
public:
  Print() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 1; }
  std::string toString() const override { return "<native fn: Print>"; }
  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class Range : public AutumnCallable,
             public std::enable_shared_from_this<Range> {
public:
  Range() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 2; }
  std::string toString() const override { return "<native fn: Range>"; }
  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class AllObjs : public AutumnCallable,
               public std::enable_shared_from_this<AllObjs> {
public:
  AllObjs() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 0; }
  std::string toString() const override { return "<native fn: AllObjs>"; }
  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class Rotate : public AutumnCallable,
              public std::enable_shared_from_this<Rotate> {
public:
  Rotate() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 1; }
  std::string toString() const override { return "<native fn: Rotate>"; }
  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class Any : public AutumnCallable,
           public std::enable_shared_from_this<Any> {
public:
  Any() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 2; }
  std::string toString() const override { return "<native fn: Any>"; }
  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class Foldl : public AutumnCallable,
             public std::enable_shared_from_this<Foldl> {
public:
  Foldl() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 3; }
  std::string toString() const override { return "<native fn: Foldl>"; }
  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class IsList : public AutumnCallable,
              public std::enable_shared_from_this<IsList> {
public:
  IsList() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 1; }
  std::string toString() const override { return "<native fn: IsList>"; }
  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class Length : public AutumnCallable,
              public std::enable_shared_from_this<Length> {
public:
  Length() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 1; }
  std::string toString() const override { return "<native fn: ListSize>"; }
  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class Head : public AutumnCallable,
            public std::enable_shared_from_this<Head> {
public:
  Head() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 1; }
  std::string toString() const override { return "<native fn: Head>"; }
  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class At: public AutumnCallable,
          public std::enable_shared_from_this<At> {
public:
  At() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 2; }
  std::string toString() const override { return "<native fn: At>"; }
  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class Tail : public AutumnCallable,
            public std::enable_shared_from_this<Tail> {
public:
  Tail() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 1; }
  std::string toString() const override { return "<native fn: Tail>"; }
  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class Prev : public AutumnCallable,
            public std::enable_shared_from_this<Prev> {
public:
  Prev() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: Prev>"; }
  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class UniformChoice : public AutumnCallable,
                     public std::enable_shared_from_this<UniformChoice> {
public:
  UniformChoice() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: UniformChoice>"; }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};
class IsOutSideBounds : public AutumnCallable,
                       public std::enable_shared_from_this<IsOutSideBounds> {
public:
  IsOutSideBounds() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override {
    return "<native fn: IsOutSideBounds>";
  }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class Clicked : public AutumnCallable,
               public std::enable_shared_from_this<Clicked> {
public:
  Clicked() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: Clicked>"; }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class DownPressed : public AutumnCallable,
                   public std::enable_shared_from_this<DownPressed> {
public:
  DownPressed() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: DownPressed>"; }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class LeftPressed : public AutumnCallable,
                   public std::enable_shared_from_this<LeftPressed> {
public:
  LeftPressed() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: LeftPressed>"; }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class RightPressed : public AutumnCallable,
                    public std::enable_shared_from_this<RightPressed> {
public:
  RightPressed() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: RightPressed>"; }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class UpPressed : public AutumnCallable,
                 public std::enable_shared_from_this<UpPressed> {
public:
  UpPressed() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: UpPressed>"; }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class ObjClicked : public AutumnCallable,
                  public std::enable_shared_from_this<ObjClicked> {
public:
  ObjClicked() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: ObjClicked>"; }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};
class AddObj : public AutumnCallable,
              public std::enable_shared_from_this<AddObj> {
public:
  AddObj() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: AddObj>"; }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class RemoveObj : public AutumnCallable,
                 public std::enable_shared_from_this<RemoveObj> {
public:
  RemoveObj() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: RemoveObj>"; }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class UpdateObj : public AutumnCallable,
                 public std::enable_shared_from_this<UpdateObj> {
public:
  UpdateObj() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: UpdateObj>"; }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class AdjPositions : public AutumnCallable,
                    public std::enable_shared_from_this<AdjPositions> {
public:
  AdjPositions() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: AdjPositions>"; }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class IsFree : public AutumnCallable,
              public std::enable_shared_from_this<IsFree> {
public:
  IsFree() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: IsFree>"; }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class Rect : public AutumnCallable,
            public std::enable_shared_from_this<Rect> {
public:
  Rect() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: Rect>"; }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class Displacement : public AutumnCallable,
                    public std::enable_shared_from_this<Displacement> {
public:
  Displacement() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: Displacement>"; }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class Adjacent : public AutumnCallable,
                public std::enable_shared_from_this<Adjacent> {
public:
  Adjacent() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: Adjacent>"; }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class AdjacentObjs : public AutumnCallable,
                    public std::enable_shared_from_this<AdjacentObjs> {
public:
  AdjacentObjs() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: AdjacentObjs>"; }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class AdjacentObjsDiagonal : public AutumnCallable,
                            public std::enable_shared_from_this<AdjacentObjsDiagonal> {
public:
  AdjacentObjsDiagonal() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override {
    return "<native fn: AdjacentObjsDiagonal>";
  }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class AdjacentDiag : public AutumnCallable,
                    public std::enable_shared_from_this<AdjacentDiag> {
public:
  AdjacentDiag() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: AdjacentDiag>"; }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class Adj : public AutumnCallable,
           public std::enable_shared_from_this<Adj> {
public:
  Adj() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: Adj>"; }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class Map : public AutumnCallable,
           public std::enable_shared_from_this<Map> {
public:
  Map() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: Map>"; }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
private:
  std::shared_ptr<AutumnValue> mapSequential(Interpreter &interpreter,
                                            std::shared_ptr<AutumnCallableValue> callable,
                                            const std::vector<std::shared_ptr<AutumnValue>> &arguments);
  std::shared_ptr<AutumnValue> mapParallelSTL(Interpreter &interpreter,
                                            std::shared_ptr<AutumnCallableValue> callable,
                                            const std::vector<std::shared_ptr<AutumnValue>> &arguments);
};

class Filter : public AutumnCallable,
              public std::enable_shared_from_this<Filter> {
public:
  Filter() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: Filter>"; }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class IsWithinBounds : public AutumnCallable,
                      public std::enable_shared_from_this<IsWithinBounds> {
public:
  IsWithinBounds() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override {
    return "<native fn: IsWithinBounds>";
  }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class RandomPositions : public AutumnCallable,
                       public std::enable_shared_from_this<RandomPositions> {
public:
  RandomPositions() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override {
    return "<native fn: RandomPositions>";
  }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class AllPositions : public AutumnCallable,
                    public std::enable_shared_from_this<AllPositions> {
public:
  AllPositions() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: AllPositions>"; }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class Concat : public AutumnCallable,
              public std::enable_shared_from_this<Concat> {
public:
  Concat() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override;
  std::string toString() const override { return "<native fn: Concat>"; }

  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class ArrayEqual : public AutumnCallable,
                  public std::enable_shared_from_this<ArrayEqual> {
public:
  ArrayEqual() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 2; }
  std::string toString() const override { return "<native fn: ArrayEqual>"; }
  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

class Sqrt : public AutumnCallable,
            public std::enable_shared_from_this<Sqrt> {
public:
  Sqrt() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;
  int arity() override { return 1; }
  std::string toString() const override { return "<native fn: Sqrt>"; }
  std::shared_ptr<AutumnCallable> clone() override {
    return shared_from_this();
  }
};

} // namespace Autumn

#endif // !AUTUMN_STD_LIB_HPP_
